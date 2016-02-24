#include "z3dsurfacefilter.h"
#include "neutubeconfig.h"

using namespace std;

//
Z3DSurfaceFilter::Z3DSurfaceFilter() :
    Z3DGeometryFilter(),
    m_showCube("Visible", true),
    m_cubeRenderer(NULL),
    m_dataIsInvalid(false),
    m_widgetsGroup(NULL)
{
    addParameter(m_showCube);

    const NeutubeConfig::Z3DWindowConfig::GraphTabConfig &config =
            NeutubeConfig::getInstance().getZ3DWindowConfig().getGraphTabConfig();
    m_showCube.set(config.isVisible());
    //  m_rendererBase->setRenderMethod("Old openGL");
    //  adjustWidgets();

    setFilterName(QString("surfacefilter"));
    //setNeedBlending(true);
    setOpacity(0.35);
}

Z3DSurfaceFilter::~Z3DSurfaceFilter()
{

}

void Z3DSurfaceFilter::initialize()
{
    Z3DGeometryFilter::initialize();

    m_cubeRenderer = new Z3DCubeRenderer();
    m_rendererBase->addRenderer(m_cubeRenderer);

    std::vector<ZParameter*> paras = m_rendererBase->getParameters();
    for (size_t i=0; i<paras.size(); i++) {
        //connect(paras[i], SIGNAL(valueChanged()), this, SLOT(invalidateResult()));
        addParameter(paras[i]);
    }
}

void Z3DSurfaceFilter::deinitialize()
{
    std::vector<ZParameter*> paras = m_rendererBase->getParameters();
    for (size_t i=0; i<paras.size(); i++) {
        removeParameter(paras[i]);
    }
    Z3DGeometryFilter::deinitialize();
}

void Z3DSurfaceFilter::setVisible(bool v)
{
    m_showCube.set(v);
}

bool Z3DSurfaceFilter::isVisible() const
{
    return m_showCube.get();
}

void Z3DSurfaceFilter::render(Z3DEye eye)
{
    if(m_cubeRenderer->isEmpty())
    {
        return;
    }

    if (!m_showCube.get())
        return;

    m_rendererBase->activateRenderer(m_cubeRenderer);

    m_rendererBase->render(eye);
}

void Z3DSurfaceFilter::process(Z3DEye)
{
    if (m_dataIsInvalid) {
        prepareData();
    }
}

void Z3DSurfaceFilter::prepareData()
{
    if (!m_dataIsInvalid)
        return;

    initialize(); // fix opengl issue

    //
    for(size_t j=0; j< m_cubeArrayList.size(); ++j)
    {

        for (size_t i = 0; i < m_cubeArrayList.at(j).size(); ++i)
        {
            Z3DCube &cube = m_cubeArrayList[j][i];

            if(cube.initByNodes)
            {
                m_cubeRenderer->addCube(&cube);
            }
            else
            {
                m_cubeRenderer->addCube(cube.length, cube.x, cube.y, cube.z, cube.color, cube.b_visible);
            }
        }
    }

    m_dataIsInvalid = false;
}

void Z3DSurfaceFilter::addData(const Z3DCube &cube)
{
    m_cubeArray.push_back(cube);

    m_dataIsInvalid = true;
    invalidateResult();
}

void Z3DSurfaceFilter::addData(ZCubeArray *cubes)
{
    m_cubeArrayList.push_back(cubes->getCubeArray());

    m_dataIsInvalid = true;
    invalidateResult();
}

void Z3DSurfaceFilter::clearData()
{
    m_cubeArray.clear();
    m_cubeArrayList.clear();
}

vector<double> Z3DSurfaceFilter::boundBox()
{
    vector<double> result(6, 0);

    for(size_t j=0; j< m_cubeArrayList.size(); ++j)
    {

        for (size_t i = 0; i < m_cubeArrayList.at(j).size(); ++i)
        {
            const Z3DCube &cube = m_cubeArrayList[j][i];

            float radius = cube.length / 2.0;

            result[0] = min(result[0], cube.x - radius);
            result[1] = max(result[1], cube.x + radius);
            result[2] = min(result[2], cube.y - radius);
            result[3] = max(result[3], cube.y + radius);
            result[4] = min(result[4], cube.z - radius);
            result[5] = max(result[5], cube.z + radius);
        }
    }

    return result;
}

ZWidgetsGroup *Z3DSurfaceFilter::getWidgetsGroup()
{
    if (!m_widgetsGroup) {
        m_widgetsGroup = new ZWidgetsGroup("Surface", NULL, 1);
        new ZWidgetsGroup(&m_showCube, m_widgetsGroup, 1);

        new ZWidgetsGroup(&m_stayOnTop, m_widgetsGroup, 1);
        std::vector<ZParameter*> paras = m_rendererBase->getParameters();
        for (size_t i=0; i<paras.size(); i++) {
            ZParameter *para = paras[i];
            if (para->getName() == "Z Scale")
                new ZWidgetsGroup(para, m_widgetsGroup, 2);
            else if (para->getName() == "Size Scale")
                new ZWidgetsGroup(para, m_widgetsGroup, 3);
            else if (para->getName() == "Rendering Method")
                new ZWidgetsGroup(para, m_widgetsGroup, 4);
            else if (para->getName() == "Opacity")
                new ZWidgetsGroup(para, m_widgetsGroup, 5);
            else
                new ZWidgetsGroup(para, m_widgetsGroup, 7);
        }
        m_widgetsGroup->setBasicAdvancedCutoff(5);
    }
    return m_widgetsGroup;
}

bool Z3DSurfaceFilter::isReady(Z3DEye eye) const
{
    //  qDebug() << "Z3DSurfaceFilter::isReady "<<Z3DGeometryFilter::isReady(eye);
    //  qDebug() << "Z3DSurfaceFilter::isReady m_showCube "<<m_showCube.get();
    //  qDebug() << "m_cubeArray isEmpty "<< m_cubeArray.empty() << "size" << m_cubeArray.size();
    return Z3DGeometryFilter::isReady(eye) && m_showCube.get() && !m_cubeArrayList.empty();
}

void Z3DSurfaceFilter::updateSurfaceVisibleState()
{
    m_dataIsInvalid = true;
    invalidateResult();
}
