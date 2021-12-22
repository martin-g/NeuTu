#ifndef FLYEMDVIDBODYMESHCACHE_H
#define FLYEMDVIDBODYMESHCACHE_H

#include <memory>

#include "flyembodymeshcache.h"

class ZDvidTarget;
class ZDvidWriter;
class ZDvidReader;

class FlyEmDvidBodyMeshCache : public FlyEmBodyMeshCache
{
public:
  FlyEmDvidBodyMeshCache();

  /*!
   * \brief Set dvid env of the cache.
   *
   * It throws a runtime exception when the cache is not set successfully.
   */
  void setDvidTarget(const ZDvidTarget &target);

  void set(const MeshIndex &index, const ZMesh *mesh) override;

  int getLatestMutationId(uint64_t bodyId) const override;

protected:
  ZMesh* getFromSolidIndex(const MeshIndex &index) const override;
  std::vector<std::string> getCachedKeys(
      uint64_t bodyId, int mutationId) const override;

private:
  ZDvidReader *getDvidReader() const;
  std::string getCacheName() const;
  std::string getBodyKey(int bodyId, int mutationId) const;
  std::string getKey(const MeshIndex &index, const std::string &format) const;

private:
  std::shared_ptr<ZDvidWriter> m_writer;
  std::string m_defaultFormat;
};

#endif // FLYEMDVIDBODYMESHCACHE_H
