namespace entity {

enum class EntityType { start, Sphere, end };

struct Entity : public Hitable, public Boundable, public ImGuiInspectable {
  INTERFACE(Entity);

  material::Material* material;

  Entity(material::Material* material) : material(material) {}

  virtual EntityType type() const = 0;
};

struct EntityList : public Hitable, public Boundable {
  typedef std::vector<Entity*>::iterator iterator;
  typedef std::vector<Entity*>::const_iterator constIterator;
  std::vector<Entity*> entities;

  EntityList() {}
  EntityList(const EntityList& other) : entities(other.entities) {}
  EntityList& operator=(const EntityList& other) {
    entities = other.entities;
    return *this;
  }

  inline Entity* at(u32 index) { return *(entities.begin() + index); }
  inline void add(Entity* entity) { entities.push_back(entity); }
  inline void remove(u32 index) {
    auto entity = entities[index];
    entities.erase(entities.begin() + index);
    delete entity;
  }
  inline size_t size() const { return entities.size(); }
  inline iterator begin() { return entities.begin(); }
  inline iterator end() { return entities.end(); }
  inline constIterator begin() const { return entities.begin(); }
  inline constIterator end() const { return entities.end(); }
  EntityList sort(u32 axis) const;
  void split(EntityList& left, EntityList& right) const;

  bool hit(const math::Ray& ray,
           const f32 tMin,
           const f32 tMax,
           Hit& resultHit) const override;

  bool bounds(math::AABB& box) const override;
};

struct Sphere : public Entity {
  math::vec3 center;
  f32 radius;

  inline EntityType type() const override { return EntityType::Sphere; }

  Sphere(math::vec3 center, f32 radius) : Sphere(center, radius, nullptr) {}
  Sphere(math::vec3 center, f32 radius, material::Material* material)
      : Entity(material), center(center), radius(radius){};

  bool hit(const math::Ray& ray,
           const f32 tMin,
           const f32 tMax,
           Hit& hit) const override;

  bool bounds(math::AABB& box) const override;

  bool renderInspector() override;
};

}  // namespace entity