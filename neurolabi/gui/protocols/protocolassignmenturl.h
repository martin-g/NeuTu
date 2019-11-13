#ifndef PROTOCOLASSIGNMENTURL_H
#define PROTOCOLASSIGNMENTURL_H

#include <QString>

class ProtocolAssignmentUrl
{
public:
    static QString GetProjects(QString server);
    static QString GetProjectsForProtocol(QString server, QString protocol);
    static QString GetEligibleProjects(QString server);
    static QString GetStartedAssigments(QString server, QString user);

    static QString GenerateAssignment(QString server, QString projectName);
    static QString StartAssignment(QString server, int assignmentID);

    static QString GetUsers(QString server);
    static QString GetJaneliaUser(QString server, QString username);

private:
    ProtocolAssignmentUrl();

    static QString AddParameter(QString url, QString parameter, QString value);
    static QString AddParameters(QString url, QMap<QString, QString>);

};

#endif // PROTOCOLASSIGNMENTURL_H
