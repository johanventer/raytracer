namespace entity {

struct Entity : public Hitable, public Boundable {
  INTERFACE(Entity);

  Scatterable* material;

  Entity(Scatterable* material) : material(material) {}
};

struct EntityList : public Hitable, public Boundable {
  std::vector<const Entity*> entities;

  EntityList() {}
  EntityList(const EntityList& other) : entities(other.entities) {}
  EntityList& operator=(const EntityList& other) {
    entities = other.entities;
    return *this;
  }

  inline void add(const Entity* entity) { entities.push_back(entity); }
  inline size_t size() const { return entities.size(); }
  void sort(u32 axis);
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

  Sphere(math::vec3 center, f32 radius, Scatterable* material)
      : Entity(material), center(center), radius(radius){};

  bool hit(const math::Ray& ray,
           const f32 tMin,
           const f32 tMax,
           Hit& hit) const override;

  bool bounds(math::AABB& box) const override;
};

}  // namespace entity