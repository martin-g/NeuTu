#ifndef ZNEURONTRACER_H
#define ZNEURONTRACER_H

#include <map>
#include <iostream>
#include <functional>
#include <stdexcept>

#include "zswcpath.h"
#include "tz_trace_defs.h"
#include "tz_trace_utils.h"
#include "common/neutudefs.h"
#include "zstackgraph.h"
#include "tz_locseg_chain.h"
#include "zprogressable.h"
//#include "geometry/zintpoint.h"
#include "zneurontracerconfig.h"

class ZStack;
class ZSwcTree;
class ZSwcConnector;
class ZJsonObject;
class ZWeightedPoint;
class ZIntPoint;

class ZNeuronTraceSeeder {
public:
  ZNeuronTraceSeeder();
  ~ZNeuronTraceSeeder();

  Stack *sortSeed(Geo3d_Scalar_Field *seedPointArray, const Stack *signal,
                Trace_Workspace *ws);

  inline std::vector<Local_Neuroseg>& getSeedArray() { return m_seedArray; }
  inline std::vector<double>& getScoreArray() { return m_seedScoreArray; }

private:
  std::vector<Local_Neuroseg> m_seedArray;
  std::vector<double> m_seedScoreArray;
};

class ZNeuronConstructor {
public:
  ZNeuronConstructor();

  inline void setWorkspace(Connection_Test_Workspace *ws) {
    m_connWorkspace = ws;
  }

  inline void setSignal(Stack *signal) {
    m_signal = signal;
  }

  ZSwcTree *reconstruct(std::vector<Locseg_Chain*> &chainArray);

private:
  Connection_Test_Workspace *m_connWorkspace;
  Stack *m_signal;
};

using ZNeuronTracerException = std::runtime_error;

class ZNeuronTracer : public ZProgressable
{
public:
  ZNeuronTracer();
  ~ZNeuronTracer();

public:
  ZSwcPath trace(double x, double y, double z);
  ZSwcTree* trace(double x, double y, double z, ZSwcTree *host);
  void updateMask(const ZSwcPath &branch);

  /*!
   * \brief Set intensity field
   * DEPRECATED. Use bindSource() instead.
   */
  void setIntensityField(ZStack *stack);

  /*!
   * \brief Bind the source stack to the tracer.
   *
   * \a stack will be used as the initial input stack for the tracing pipleine.
   * Note that the source can only be bound once. It will throw
   * ZNeuronTracerException if the binding fails.
   */
  void bindSource(ZStack *stack);

  Stack* getIntensityData() const;
  inline const ZStack *getStack() const { return m_stack; }
  inline ZStack *getStack() { return m_stack; }
  void updateMask(Swc_Tree *tree);
  void setTraceWorkspace(Trace_Workspace *workspace);
  void setConnWorkspace(Connection_Test_Workspace *workspace);

  void configure();

  Swc_Tree* trace(double x1, double y1, double z1, double r1,
                 double x2, double y2, double z2, double r2);

  void setTraceRange(const ZIntCuboid &box);

  void clear();

  inline void setBackgroundType(neutu::EImageBackground bg) {
    m_backgroundType = bg;
  }

  inline void setResolution(double x, double y, double z) {
    m_resolution[0] = x;
    m_resolution[1] = y;
    m_resolution[2] = z;
  }

#if 0
  inline void setStackOffset(double x, double y, double z) {
    m_stackOffset[0] = x;
    m_stackOffset[1] = y;
    m_stackOffset[2] = z;
  }

  void setStackOffset(const ZIntPoint &pt);
#endif

  inline void setVertexOption(ZStackGraph::EVertexOption vertexOption) {
    m_vertexOption = vertexOption;
  }

  void setSignalChannel(int c) { m_preferredSignalChannel = c; }
  int getSignalChannel() const { return m_preferredSignalChannel; }

  /*!
   * \brief Auto trace
   *
   * It will also create workspaces automatically if necessary.
   */
  ZSwcTree* trace(Stack *stack, bool doResampleAfterTracing = true);

  ZSwcTree* trace(const ZStack *stack, bool doResampleAfterTracing = true);

  ZSwcTree* trace();

  ZSwcTree* connectBranch(const ZSwcPath &branch, ZSwcTree *host);

  //Autotrace configuration
  //Trace level setup: 1 - 10 (fast -> accurate)
  void setTraceLevel(int level);


  //Helper functions
  static double findBestTerminalBreak(
      const ZPoint &terminalCenter, double terminalRadius,
      const ZPoint &innerCenter, double innerRadius,
      const Stack *stack);

  Trace_Workspace* getTraceWorkspace();
  Connection_Test_Workspace* getConnectionTestWorkspace();

  void initTraceMask(bool clearing);

  /*!
   * \brief Add more masked regions to the tracing mask.
   *
   * The functions sets the background of the current mask to the corresponding
   * values in \a stack. Nothing will be done if \a stack is NULL or the tracer
   * has no workspace set up. It throws a ZNeuronTracerException if the size of
   * \a stack does not match the size of the bound signal stack or its kind is
   * not GREY.
   *
   * Note that this function will create the tracing mask if it does not exist
   * yet.
   */
  void addTraceMask(const Stack *stack);

  Stack* getTraceMask() const;

  void initTraceWorkspace(Stack *stack);
  void initTraceWorkspace(ZStack *stack);
  void initConnectionTestWorkspace();

  void updateTraceWorkspace(int traceEffort, bool traceMasked,
                            double xRes, double yRes, double zRes);
  void updateTraceWorkspaceResolution(
      double xRes, double yRes, double zRes);
  void updateConnectionTestWorkspace(
      double xRes, double yRes, double zRes,
      char unit, double distThre, bool spTest, bool crossoverTest);

  void loadTraceMask(bool traceMasked);


  void loadJsonObject(const ZJsonObject &obj);


  enum ETracingMode {
    TRACING_AUTO, TRACING_INTERACTIVE, TRACING_SEED
  };
  void prepareTraceScoreThreshold(ETracingMode mode);
  void setMinScore(double score, ETracingMode mode);

  void useEdgePath(bool state) {
    m_usingEdgePath = state;
  }

  void setBcAdjust(bool on) { m_bcAdjust = on; }
  void setGreyFactor(double v) { m_greyFactor = v; }
  void setGrayOffset(double v) { m_greyOffset = v; }
  void setEstimatingRadius(bool on) { m_estimatingRadius = on; }

  void setDiagnosis(bool on) { m_diagnosis = on; }

  void setLogger(std::function<void(const std::string&)> f) {
    m_log = f;
  }

  void enableTraceMask(bool on);
  void setOverTrace(bool on);
  void setSeedScreening(bool on);
  void setChainScreenCount(int c);

public:
  std::vector<ZWeightedPoint> computeSeedPosition(const Stack *stack);
  std::vector<ZWeightedPoint> computeSeedPosition(const ZStack *stack);
  std::vector<ZWeightedPoint> computeSeedPosition();
  ZSwcTree *computeInitialTrace(const Stack *stack);

  int getRecoverLevel() const;
  void setRecoverLevel(int level);

  Stack* computeSeedMask();
  Stack* computeSeedMask(Stack *stack);

  Geo3d_Scalar_Field* removeNoisySeed(Geo3d_Scalar_Field *seedPointArray,
      Stack *mask);

  ZNeuronTracerConfig* getConfigRef() {
    return &m_config;
  }

  friend std::ostream& operator << (std::ostream &stream, const ZNeuronTracer &t);

  Stack* makeMask(const Stack *stack);

public:
  void test();

public: //pipeline functions
  std::function<void(Stack*)> _preprocess;
  std::function<Stack*(Stack*)> _makeMask;
  std::function<Geo3d_Scalar_Field*(Stack*)> _extractSeedFromMask;

private:
  //Helper functions
  Stack* binarize(const Stack *stack, Stack *out = NULL);
  Stack* bwsolid(Stack *stack);
  Stack* enhanceLine(const Stack *stack);
  Geo3d_Scalar_Field* extractSeed(const Stack *mask);
  Geo3d_Scalar_Field* extractSeedOriginal(const Stack *mask);
  Geo3d_Scalar_Field* extractSeedSkel(const Stack *mask);

  Geo3d_Scalar_Field* extractLineSeed(
      const Stack *mask, const Stack *dist, int minObjSize = 0);

  ZSwcTree *reconstructSwc(const Stack *stack,
                           std::vector<Locseg_Chain*> &chainArray);
  std::vector<Locseg_Chain*> trace(const Stack *stack,
                                   std::vector<Local_Neuroseg> &locsegArray,
                                   std::vector<double> &values);
  std::vector<Locseg_Chain*> recover(const Stack *stack);

  std::vector<Locseg_Chain*> screenChain(const Stack *stack,
                                         std::vector<Locseg_Chain*> &chainArray);

  void clearBuffer();

  void init();

  int getMinSeedObjSize(double seedDensity) const;
  bool traceMasked(int x, int y, int z) const;

  Geo3d_Scalar_Field* removeTracedSeed(Geo3d_Scalar_Field *seedArray);

  std::string getDiagnosisDir() const;
  void log(const std::string &str);

  class Diagnosis {
  public:
    Diagnosis();
    Diagnosis(const std::string &dir);

    void reset();
    void setDir(const std::string &dir);
    void setPrefix(const std::string &prefix);
    std::string getDir() const;
    std::string getPrefix() const;
    void saveConfig(const ZNeuronTracer &tracer) const;
    void save(const ZStack *stack, const std::string &name) const;
    void save(const Stack *stack, const std::string &name) const;
    void save(const Geo3d_Scalar_Field *field, const std::string &name) const;
    void save(const std::vector<Locseg_Chain*> &chainArray,
              const std::string &name) const;
    void save(ZSwcTree *tree, const std::string &name) const;
    void saveInfo() const;
    void setInfo(const std::string &key, const std::string &value);
    void setInfo(const std::string &key, int value);
  private:
    std::string m_dir;
    ZJsonObject m_info;
    std::string m_prefix;
  };

private:
  ZStack *m_stack;
  Trace_Workspace *m_traceWorkspace;
  Connection_Test_Workspace *m_connWorkspace;
  ZSwcConnector *m_swcConnector;
  neutu::EImageBackground m_backgroundType;
  ZStackGraph::EVertexOption m_vertexOption;
  double m_resolution[3];

  ZSwcTree *m_initialSwc = nullptr;
//  double m_stackOffset[3];

  double m_seedMinScore;
  double m_autoTraceMinScore;
  double m_traceMinScore;
  double m_2dTraceMinScore;
  bool m_usingEdgePath;
  bool m_enhancingMask;

  int m_recover;
  int m_seedingMethod;
  int m_preferredSignalChannel;
  int m_chainScreenCount = 0;

  //Intermedite buffer
  std::vector<Locseg_Chain*> m_chainArray;
  Stack *m_mask;
  Stack *m_baseMask;
//  ZIntPoint m_seedDsIntv;

  bool m_bcAdjust;
  double m_greyFactor;
  double m_greyOffset;
  bool m_estimatingRadius;
  bool m_maskTracing = true;
  bool m_diagnosis = false;
  bool m_screeningSeed = true;

  ZNeuronTracerConfig m_config; //default configuration
  Diagnosis m_diag;
  std::function<void(const std::string)> m_log =
      [](const std::string &str) { std::cout << str << std::endl; };

  /*
  static const char *m_levelKey;
  static const char *m_minimalScoreKey;
  static const char *m_minimalSeedScoreKey;
  static const char *m_spTestKey;
  static const char *m_enhanceLineKey;
  */
};

#endif // ZNEURONTRACER_H
