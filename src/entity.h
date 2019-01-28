namespace entity {

enum class EntityType {
  start,
  Sphere,
  XYRectangle,
  XZRectangle,
  YZRectangle,
  FlipNormals,
  Box,
  end
};

struct Entity : public Hitable, public Boundable, public ImGuiInspectable {
  INTERFACE(Entity);
  material::Material* material = nullptr;
  virtual EntityType type() const = 0;
  Entity(material::Material* material) : material(material) {}
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
  inline void clear() { entities.clear(); }
  EntityList sort(u32 axis) const;
  void split(EntityList& left, EntityList& right) const;

  bool hit(const math::Ray& ray,
           const f32 tMin,
           const f32 tMax,
           Hit& resultHit) const override;

  bool bounds(math::AABB& box) const override;
};

struct Sphere : public Entity {
  math::vec3 center = {0, 0, 0};
  f32 radius = 1;

  inline EntityType type() const override { return EntityType::Sphere; }

  Sphere() {}
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

struct XYRectangle : public Entity {
  math::vec3 center = {0, 1, 0};
  f32 width = 1;
  f32 height = 1;

  inline EntityType type() const override { return EntityType::XYRectangle; }

  XYRectangle() {}
  XYRectangle(math::vec3 center,
              f32 width,
              f32 height,
              material::Material* material)
      : Entity(material), center(center), width(width), height(height) {}

  bool hit(const math::Ray& ray,
           const f32 tMin,
           const f32 tMax,
           Hit& hit) const override;
  bool bounds(math::AABB& box) const override;
  bool renderInspector() override;
};

struct XZRectangle : public Entity {
  math::vec3 center = {0, 1, 0};
  f32 width = 1;
  f32 height = 1;

  inline EntityType type() const override { return EntityType::XZRectangle; }

  XZRectangle() {}
  XZRectangle(math::vec3 center,
              f32 width,
              f32 height,
              material::Material* material)
      : Entity(material), center(center), width(width), height(height) {}

  bool hit(const math::Ray& ray,
           const f32 tMin,
           const f32 tMax,
           Hit& hit) const override;
  bool bounds(math::AABB& box) const override;
  bool renderInspector() override;
};

struct YZRectangle : public Entity {
  math::vec3 center = {0, 1, 0};
  f32 width = 1;
  f32 height = 1;

  inline EntityType type() const override { return EntityType::YZRectangle; }

  YZRectangle() {}
  YZRectangle(math::vec3 center,
              f32 width,
              f32 height,
              material::Material* material)
      : Entity(material), center(center), width(width), height(height) {}

  bool hit(const math::Ray& ray,
           const f32 tMin,
           const f32 tMax,
           Hit& hit) const override;
  bool bounds(math::AABB& box) const override;
  bool renderInspector() override;
};

struct Box : public Entity {
  math::vec3 center = {0, 0, 0};
  f32 width = 1;
  f32 height = 1;
  f32 depth = 1;
  EntityList entities;

  inline EntityType type() const override { return EntityType::Box; }

  Box() { construct(); }
  Box(math::vec3 center,
      f32 width,
      f32 height,
      f32 depth,
      material::Material* material)
      : Entity(material),
        center(center),
        width(width),
        height(height),
        depth(depth) {
    construct();
  }

  void construct();

  bool hit(const math::Ray& ray,
           const f32 tMin,
           const f32 tMax,
           Hit& hit) const override {
    return entities.hit(ray, tMin, tMax, hit);
  }
  bool bounds(math::AABB& box) const override {
    // TODO(johan): This is trivial to calculate for the box, no need to
    // delegate to something far more generic.
    return entities.bounds(box);
  };
  bool renderInspector() override { return false; }
};

struct FlipNormals : public Entity {
  Entity* entity = nullptr;

  inline EntityType type() const override { return EntityType::FlipNormals; }

  FlipNormals() {}
  FlipNormals(Entity* entity) : entity(entity) {}

  bool hit(const math::Ray& ray,
           const f32 tMin,
           const f32 tMax,
           Hit& hit) const override {
    if (entity && entity->hit(ray, tMin, tMax, hit)) {
      hit.normal = -hit.normal;
      return true;
    }
    return false;
  }
  bool bounds(math::AABB& box) const override {
    return entity && entity->bounds(box);
  }
  bool renderInspector() override { return false; }
};

}  // namespace entity