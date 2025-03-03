#ifndef ZFLYEMBODYANNOTATION_H
#define ZFLYEMBODYANNOTATION_H

#include <cstdint>
#include <string>
#include <functional>

class ZJsonObject;

/*!
 * \brief The class of body annotation
 */
class ZFlyEmBodyAnnotation
{
public:
  ZFlyEmBodyAnnotation();

//  inline uint64_t getBodyId() const { return m_bodyId; }
  inline const std::string& getStatus() const { return m_status; }
  inline const std::string& getComment() const { return m_comment; }
//  inline const std::string& getName() const { return m_name; }
  inline const std::string& getType() const { return m_type; }
  inline const std::string& getUser() const { return m_userName; }
  inline const std::string& getNamingUser() const { return m_namingUser; }
  inline const std::string& getStatusUser() const { return m_statusUser; }

  std::string getMajorInput() const;
  std::string getMajorOutput() const;
  std::string getPrimaryNeurite() const;
  std::string getLocation() const;
  bool getOutOfBounds() const;
  bool getCrossMidline() const;
  std::string getNeurotransmitter() const;
  std::string getSynonym() const;
  std::string getHemilineage() const;
  std::string getClonalUnit() const;
  std::string getProperty() const;

  std::string getName() const;

  std::string getClass() const;
  std::string getInferredType() const;
  std::string getAutoType() const;
  int64_t getTimestamp() const;
//  std::string getAutoName() const;

//  inline void setBodyId(uint64_t bodyId) { m_bodyId = bodyId; }
//  void setBodyId(int bodyId);
//  void setBodyId(int64_t bodyId);
  inline void setStatus(const std::string &status) {
    m_status = status;
  }
  inline void setName(const std::string &name) { m_name = name; }
  inline void setClass(const std::string &c) { m_class = c; }
  inline void setComment(const std::string &comment) { m_comment = comment; }
  inline void setUser(const std::string &user) { m_userName = user; }
  inline void setNamingUser(const std::string &user) { m_namingUser = user; }
  inline void setStatusUser(const std::string &user) { m_statusUser = user; }
  inline void setInstance(const std::string &instance) { m_instance = instance; }
  void setMajorInput(const std::string &v);
  void setMajorOutput(const std::string &v);
  void setPrimaryNeurite(const std::string &v);
  void setLocation(const std::string &v);
  void setOutOfBounds(bool v);
  void setCrossMidline(bool v);
  void setNeurotransmitter(const std::string &v);
  void setHemilineage(const std::string &v);
  void setSynonym(const std::string &v);
  void setClonalUnit(const std::string &v);
  void setAutoType(const std::string &v);
  void setProperty(const std::string &v);

  void updateTimestamp();

  /*!
   * \brief Add a timestamp field to a json object
   *
   * This function can be used to update timestamp in an annotation json.
   */
  static void UpdateTimeStamp(ZJsonObject &obj);

  /*!
   * \brief Load the data from a json string
   *
   * {
   *   "status": string,
   *   "comment": string,
   *   "body ID": int,
   *   "name": string,
   *   "class": string,
   *   "user": string,
   *   "naming user": string
   * }
   *
   * A property is cleared if the corresponding field does not exist.
   */
  void loadJsonString(const std::string &str);

  void loadJsonObject(const ZJsonObject &obj);

  ZJsonObject toJsonObject() const;

  void clear();
  void print() const;

  bool isEmpty() const;

  void mergeAnnotation(
      const ZFlyEmBodyAnnotation &annotation,
      const std::function<int(const std::string&)> &getStatusRank);

  std::string brief(uint64_t bodyId = 0) const;

  bool isFinalized() const;

  bool hasSameUserStatus(const ZFlyEmBodyAnnotation &annot) const;
  bool operator == (const ZFlyEmBodyAnnotation &annot) const;

public:
  static int GetStatusRank(const std::string &status);
  static std::string GetName(const ZJsonObject &obj);
  static std::string GetStatus(const ZJsonObject &obj);
//  static uint64_t GetBodyId(const ZJsonObject &obj);
  static std::string GetClass(const ZJsonObject &obj);
  static std::string GetUser(const ZJsonObject &obj);
  static std::string GetNamingUser(const ZJsonObject &obj);
  static std::string GetLastModifiedBy(const ZJsonObject &obj);

  static std::string GetName(const ZFlyEmBodyAnnotation &obj);
  static std::string GetStatus(const ZFlyEmBodyAnnotation &obj);
  static std::string GetClass(const ZFlyEmBodyAnnotation &obj);
  static std::string GetType(const ZFlyEmBodyAnnotation &obj);

  static std::string GetComment(const ZJsonObject &obj);
  static void SetComment(
      ZJsonObject &obj, const std::string &comment, bool usingDescription);

  static void SetStatus(ZJsonObject &obj, const std::string &status);
  static void SetUser(ZJsonObject &obj, const std::string &user);
  static void SetLastModifiedBy(ZJsonObject &obj, const std::string &user);
  static void SetNamingUser(ZJsonObject &obj, const std::string &user);
  static void UpdateUserFields(
      ZJsonObject &obj, const std::string &user, const ZJsonObject &oldObj);
  static std::string GetType(const ZJsonObject &obj);
//  static void SetBodyId(ZJsonObject &obj, const uint64_t bodyId);

  static std::string Brief(uint64_t bodyId, const ZJsonObject &obj);
  static std::string Brief(uint64_t bodyId, const ZFlyEmBodyAnnotation &obj);

  static bool IsFinalized(const ZJsonObject &obj);

  static ZJsonObject MergeAnnotation(
      const ZJsonObject &target, const ZJsonObject &source,
      const std::function<int(const std::string&)>& getStatusRank);

  static const char *KEY_BODY_ID;
  static const char *KEY_STATUS;
  static const char *KEY_NAME;
  static const char *KEY_CLASS;
  static const char *KEY_TYPE;
  static const char *KEY_COMMENT;
  static const char *KEY_DESCRIPTION;
  static const char *KEY_USER;
  static const char *KEY_LAST_MODIFIED_USER;
  static const char *KEY_NAMING_USER_OLD;
  static const char *KEY_NAMING_USER;
  static const char *KEY_STATUS_USER;
  static const char *KEY_INSTANCE;
  static const char *KEY_MAJOR_INPUT;
  static const char *KEY_MAJOR_OUTPUT;
  static const char *KEY_PRIMARY_NEURITE;
  static const char *KEY_CELL_BODY_FIBER;
  static const char *KEY_LOCATION;
  static const char *KEY_OUT_OF_BOUNDS;
  static const char *KEY_CROSS_MIDLINE;
  static const char *KEY_NEURONTRANSMITTER;
  static const char *KEY_HEMILINEAGE;
  static const char *KEY_SYNONYM;
  static const char *KEY_NOTES;
  static const char *KEY_CLONAL_UNIT;
  static const char *KEY_AUTO_TYPE;
  static const char *KEY_PROPERTY;
  static const char *KEY_TIMESTAMP;

private:
  static std::string GetOldFormatKey(const ZJsonObject &obj);

private:
  uint64_t m_bodyId = 0;
  std::string m_status;
  std::string m_comment;
  std::string m_name;
  std::string m_class;
  std::string m_type;
  std::string m_userName;
  std::string m_namingUser;
  std::string m_statusUser;

  std::string m_instance;
  std::string m_property;
  std::string m_majorInput;
  std::string m_majorOutput;
  std::string m_primaryNeurite;
  std::string m_location;
  bool m_outOfBounds = false;
  bool m_crossMidline = false;
  std::string m_neurotransmitter;
  std::string m_hemilineage;
  std::string m_synonym;
  std::string m_clonalUnit;
  std::string m_autoType;
  int64_t m_timestamp = 0;
};

#endif // ZFLYEMBODYANNOTATION_H
