#include "taskprotocoltask.h"

#include <QsLog.h>

/*
 * this is the abstract base class for tasks used by the TaskProtocolWindow in Neu3
 *
 */
TaskProtocolTask::TaskProtocolTask()
{

    m_completed = false;

}

// constants
const QString TaskProtocolTask::KEY_COMPLETED = "completed";
const QString TaskProtocolTask::KEY_TAGS = "tags";
const QString TaskProtocolTask::KEY_VISIBLE = "visible";
const QString TaskProtocolTask::KEY_SELECTED= "selected";

bool TaskProtocolTask::completed() const
{
    return m_completed;
}

QSet<uint64_t> TaskProtocolTask::visibleBodies() {
    return m_visibleBodies;
}

QSet<uint64_t> TaskProtocolTask::selectedBodies() {
    return m_selectedBodies;
}

void TaskProtocolTask::setCompleted(bool completed)
{
    m_completed = completed;
}

/*
 * parse out the standard fields that are used in all tasks
 */
bool TaskProtocolTask::loadStandard(QJsonObject json) {
    // these keys are optional, so no need to fail if missing;
    //  we'll take the default values

    if (json.contains(KEY_COMPLETED)) {
        m_completed = json[KEY_COMPLETED].toBool();
    }

    m_visibleBodies.clear();
    if (json.contains(KEY_VISIBLE)) {
        foreach (QJsonValue value, json[KEY_VISIBLE].toArray()) {
            m_visibleBodies.insert(value.toDouble());
        }
    }

    m_selectedBodies.clear();
    if (json.contains(KEY_SELECTED)) {
        foreach (QJsonValue value, json[KEY_SELECTED].toArray()) {
            m_selectedBodies.insert(value.toDouble());
        }
    }

    // if there are selected or visible bodies, though, we
    //  have to check them
    // this is a little iffy; our body IDs are 64-bit; json numbers
    //  are floats and technnically not limited; however, Qt only
    //  converts them to doubles; that means if we have a body ID
    //  above about 2^52, we won't be able to convert it without
    //  losing precision; that seems unlikely in the near future,
    //  so test and log, but don't handle it yet
    // alternate possibility is to store the body IDs as strings are parse,
    //  but that seems just as inelegant
    QSet<uint64_t> allBodies;
    allBodies.unite(m_visibleBodies);
    allBodies.unite(m_selectedBodies);
    if (allBodies.size() > 0) {
        foreach (uint64_t bodyID, allBodies) {
            if (bodyID == 0) {
                // 0 indicates a conversion failure; we don't
                //  anticipate reviewing body 0
                LINFO() << "error converting task json; body ID = 0 not allowed";
                return false;
            }
            if (bodyID > 4503599627370496) {
                // that number is 2^52
                LINFO() << "error converting task json; found body ID > 2^52";
                return false;
            }
        }
    }


    // m_tags.clear();
    // if (json.contains)

    return true;
}

/*
 * utility; when having trouble with input json, nice
 * to embed that json in the error
 */
QString TaskProtocolTask::objectToString(QJsonObject json) {
    QJsonDocument doc(json);
    QString jsonString(doc.toJson(QJsonDocument::Compact));
    return jsonString;
}

QJsonObject TaskProtocolTask::toJson() {
    // see note in loadJson() re: precision; because we
    //  know the source of the body IDs, the conversions
    //  below should be OK

    QJsonObject taskJson;

    taskJson[KEY_COMPLETED] = m_completed;

    if (m_visibleBodies.size() > 0) {
        QJsonArray array;
        foreach (uint64_t bodyID, m_visibleBodies) {
            array.append(static_cast<double>(bodyID));
        }
        taskJson[KEY_VISIBLE] = array;
    }
    if (m_selectedBodies.size() > 0) {
        QJsonArray array;
        foreach (uint64_t bodyID, m_selectedBodies) {
            array.append(static_cast<double>(bodyID));
        }
        taskJson[KEY_SELECTED] = array;
    }

    // allow subclass to add fiels:
    taskJson = addToJson(taskJson);

    return taskJson;
}

bool TaskProtocolTask::loadJson(QJsonObject json) {

    // take care of standard fields
    if (!loadStandard(json)) {
        return false;
    }

    // and fields provided by subclasses
    if (!loadSpecific(json)) {
        return false;
    }

    return true;
}
