/*
 * Copyright (c) 2010 Novell, Inc.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; version 3 of the license.
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail,
 * you may find current contact information at www.novell.com
 *
 * Author: Matt Barringer <mbarringer@suse.de>
 * Upstream: http://qsusestudio.sourceforge.net
 *
 */


/* NOTE: THIS WILL ONLY WORK WITH QT 4.6 AND ABOVE */

#include "qsusestudio.h"

#include <QXmlStreamAttributes>
#include <QDebug>

QSUSEStudio::QSUSEStudio(const QString &username,
                         const QString &key,
                         const QString &url)
    : QObject(),  mApiKey(key), mUsername(username), mUrl(url)
{
    pManager = new QNetworkAccessManager(this);
    connect(pManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(giveCredentials(QNetworkReply*,QAuthenticator*)));
}

QSUSEStudio::~QSUSEStudio()
{
    delete pManager;
}

void
QSUSEStudio::getAccountInfo()
{
    QString request = QString("%1/api/v1/user/account").arg(mUrl);
    getRequest(request, GET_ACCOUNT_INFO);
}

void
QSUSEStudio::getTemplateSets()
{
    QString request = QString("%1/api/v1/user/template_sets").arg(mUrl);
    getRequest(request, GET_TEMPLATE_SETS);
}

void
QSUSEStudio::getTemplateSets(const QString &name)
{
    QString request = QString("%1/api/v1/user/template_sets/%2").arg(mUrl).arg(name);
    getRequest(request, GET_TEMPLATE_SETS);
}

void
QSUSEStudio::createAppliance(const QString &templateId,
                             const QString &name,
                             const QString &arch)
{
    QString request = QString("%1/api/v1/user/appliances?clone_from=%2&name=%3&arch=%4")
                             .arg(mUrl)
                             .arg(templateId)
                             .arg(name)
                             .arg(arch);
    QByteArray empty;
    postRequest(request, empty, CREATE_APPLIANCE);
}

void
QSUSEStudio::listAppliances()
{
    QString request = QString("%1/api/v1/user/appliances").arg(mUrl);
    getRequest(request, LIST_APPLIANCES);
}

void
QSUSEStudio::getApplianceDetails(const QString &id)
{
    QString request = QString("%1/api/v1/user/appliances/%2").arg(mUrl).arg(id);
    getRequest(request, GET_APPLIANCE_DETAILS);
}

void
QSUSEStudio::getApplianceStatus(const QString &id)
{
    QString request = QString("%1/api/v1/user/appliances/%2/status").arg(mUrl).arg(id);
    getRequest(request, GET_APPLIANCE_STATUS);
}

void
QSUSEStudio::deleteAppliance(const QString &applianceId)
{
    QString request = QString("%1/api/v1/user/appliances/%2").arg(mUrl).arg(applianceId);
    deleteRequest(request, DELETE_APPLIANCE);
}

void
QSUSEStudio::listApplianceRepositories(const QString &applianceId)
{
    QString request = QString("%1/api/v1/user/appliances/%2/repositories").arg(mUrl).arg(applianceId);
    getRequest(request, LIST_APPLIANCE_REPOSITORIES);
}

void
QSUSEStudio::setApplianceRepositories(const QString &applianceId,
                                      const QStringList &repositoryList)
{
    QString request = QString("%1/api/v1/user/appliances/%2/repositories").arg(mUrl).arg(applianceId);
    QByteArray data;
    data.append("<repositories>");
    for (int i = 0; i < repositoryList.size(); ++i)
    {
          data.append("<repository><id>");
          data.append(repositoryList.at(i).toLocal8Bit().constData());
          data.append("</id></repository>");
    }
    data.append("</repositories>");
    putRequest(request, data, SET_APPLIANCE_REPOSITORIES);
}

void
QSUSEStudio::addApplianceRepository(const QString &applianceId,
                                    const QString &repositoryId)
{
    QString request = QString("%1/api/v1/user/appliances/%2/cmd/add_repository?repo_id=%3").arg(mUrl).arg(applianceId).arg(repositoryId);
    QByteArray empty;
    postRequest(request, empty, ADD_APPLIANCE_REPOSITORY);
}

void
QSUSEStudio::removeApplianceRepository(const QString &applianceId,
                                       const QString &repositoryId)
{
    QString request = QString("%1/api/v1/user/appliances/%2/cmd/remove_repository?repo_id=%3").arg(mUrl).arg(applianceId).arg(repositoryId);
    QByteArray empty;
    postRequest(request, empty, REMOVE_APPLIANCE_REPOSITORY);
}

void
QSUSEStudio::addApplianceUserRepository(const QString &applianceId)
{
    QString request = QString("%1/api/v1/user/appliances/%2/cmd/add_user_repository").arg(mUrl).arg(applianceId);
    QByteArray empty;
    postRequest(request, empty, REMOVE_APPLIANCE_REPOSITORY);
}

void
QSUSEStudio::updateApplianceSoftware(const QString &applianceId)
{
    QString request = QString("%1/api/v1/user/appliances/%2/software").arg(mUrl).arg(applianceId);
    QByteArray empty;
    putRequest(request, empty, UPDATE_APPLIANCE_SOFTWARE);
}

void
QSUSEStudio::addApplianceSoftware(const QString &applianceId,
                                  const QString &packageName,
                                  const QString &packageVersion,
                                  const QString &packageRepositoryId)
{
    QString request = QString("%1/api/v1/user/appliances/%2/cmd/add_package?name=%3&version=%4&repository_id=%5")
                             .arg(mUrl)
                             .arg(applianceId)
                             .arg(packageName)
                             .arg(packageVersion)
                             .arg(packageRepositoryId);
    QByteArray empty;
    postRequest(request, empty, ADD_APPLIANCE_SOFTWARE);
}

void
QSUSEStudio::addAppliancePattern(const QString &applianceId,
                                 const QString &patternName,
                                 const QString &patternVersion,
                                 const QString &patternRepositoryId)
{
    QString request = QString("%1/api/v1/user/appliances/%2/cmd/add_pattern?name=%3&version=%4&repository_id=%5")
                             .arg(mUrl)
                             .arg(applianceId)
                             .arg(patternName)
                             .arg(patternVersion)
                             .arg(patternRepositoryId);
    QByteArray empty;
    postRequest(request, empty, ADD_APPLIANCE_PATTERN);
}


void
QSUSEStudio::removeApplianceSoftware(const QString &applianceId,
                                     const QString &packageName)
{
    QString request = QString("%1/api/v1/user/appliances/%2/cmd/remove_package?name=%3")
                             .arg(mUrl)
                             .arg(applianceId)
                             .arg(packageName);
    QByteArray empty;
    postRequest(request, empty, REMOVE_APPLIANCE_SOFTWARE);
}


void
QSUSEStudio::removeAppliancePattern(const QString &applianceId,
                                    const QString &patternName)
{
    QString request = QString("%1/api/v1/user/appliances/%2/cmd/remove_pattern?name=%3")
                             .arg(mUrl)
                             .arg(applianceId)
                             .arg(patternName);
    QByteArray empty;
    postRequest(request, empty, REMOVE_APPLIANCE_PATTERN);
}

void
QSUSEStudio::banApplianceSoftware(const QString &applianceId,
                                  const QString &packageName)
{
    QString request = QString("%1/api/v1/user/appliances/%2/cmd/ban_package?name=%3")
                             .arg(mUrl)
                             .arg(applianceId)
                             .arg(packageName);
    QByteArray empty;
    postRequest(request, empty, BAN_APPLIANCE_SOFTWARE);
}

void
QSUSEStudio::unbanApplianceSoftware(const QString &applianceId,
                                    const QString &packageName)
{
    QString request = QString("%1/api/v1/user/appliances/%2/cmd/unban_package?name=%3")
                             .arg(mUrl)
                             .arg(applianceId)
                             .arg(packageName);
    QByteArray empty;
    postRequest(request, empty, UNBAN_APPLIANCE_SOFTWARE);
}

void
QSUSEStudio::listApplianceSoftware(const QString &id)
{
    QString request = QString("%1/api/v1/user/appliances/%2/software").arg(mUrl).arg(id);
    getRequest(request, LIST_APPLIANCE_SOFTWARE);
}

void
QSUSEStudio::listInstalledSoftware(const QString &id,
                                  const QString &buildId)
{
    QString request = QString("%1/api/v1/user/appliances/%2/software/installed?build_id=%3").arg(mUrl).arg(id).arg(buildId);
    getRequest(request, LIST_APPLIANCE_INSTALLED_SOFTWARE);
}

void
QSUSEStudio::searchSoftware(const QString &id,
                            const QString &query,
                            bool allFields,
                            bool allRepos)
{
    QString request = QString("%1/api/v1/user/appliances/%2/software/search?q=%3&all_fields=%4&all_repos=%5")
                             .arg(mUrl)
                             .arg(id)
                             .arg(query)
                             .arg(allFields)
                             .arg(allRepos);
    getRequest(request, SEARCH_SOFTWARE);
}

void
QSUSEStudio::getImageFile(const QString &id, const QString &buildId, const QString &path)
{
    QString request = QString("%1/api/v1/user/appliances/%2/image_files?build_id=%3&path=%4")
                             .arg(mUrl)
                             .arg(id)
                             .arg(buildId)
                             .arg(path);

    getRequest(request, GET_IMAGE_FILE);
}

void
QSUSEStudio::listApplianceGPGKeys(const QString &id, const QString &keyId)
{
    QString request = QString("%1/api/v1/user/appliances/%2/gpg_keys").arg(mUrl).arg(id);
    if (keyId != "")
        request += "/" + keyId;
    getRequest(request, LIST_APPLIANCE_GPG_KEYS);
}

void
QSUSEStudio::addApplianceGPGKey(const QString &applianceId,
                                const QString &keyName,
                                const QString &key,
                                const QString &target)
{
    QString request = QString("%1/api/v1/user/appliances/%2/gpg_keys?name=%3&target=%4")
                             .arg(mUrl)
                             .arg(applianceId)
                             .arg(keyName)
                             .arg(target);
    QByteArray keyArray;
    keyArray.append(key);
    postRequest(request, keyArray, ADD_APPLIANCE_GPG_KEY);
}

void
QSUSEStudio::removeApplianceGPGKey(const QString &applianceId,
                                   const QString &keyId)
{
    QString request = QString("%1/api/v1/user/appliances/%2/gpg_keys/%3")
                             .arg(mUrl)
                             .arg(applianceId)
                             .arg(keyId);
    QByteArray empty;
    deleteRequest(request, REMOVE_APPLIANCE_GPG_KEY);
}

void
QSUSEStudio::listOverlayFiles(const QString &id)
{
    QString request = QString("%1/api/v1/user/files?appliance_id=%2").arg(mUrl).arg(id);
    getRequest(request, LIST_OVERLAY_FILES);
}

void
QSUSEStudio::getOverlayFile(const QString &id)
{
    QString request = QString("%1/api/v1/user/files/%2/data").arg(mUrl).arg(id);
    getRequest(request, GET_OVERLAY_FILE);
}

void
QSUSEStudio::getOverlayFileMetadata(const QString &id)
{
    QString request = QString("%1/api/v1/user/files/%2").arg(mUrl).arg(id);
    getRequest(request, GET_OVERLAY_FILE_METADATA);
}

void
QSUSEStudio::addOverlayFile(const QString &applianceId,
                            const QByteArray &fileData,
                            const QString &name,
                            const QString &path,
                            const QString &owner,
                            const QString &group,
                            const QString &permissions,
                            const QString &enabled)
{
    QString request = QString("%1/api/v1/user/files?appliance_id=%2&filename=%3&path=%4&owner=%5&group=%6&permissions=%7&enabled=%8")
                             .arg(mUrl)
                             .arg(applianceId)
                             .arg(name)
                             .arg(path)
                             .arg(owner)
                             .arg(group)
                             .arg(permissions)
                             .arg(enabled);
    postRequest(request, fileData, ADD_OVERLAY_FILE);
}

void
QSUSEStudio::updateOverlayFile(const QString &fileId,
                               const QByteArray &fileData)
{
    QString request = QString("%1/api/v1/user/files/%2/data")
                             .arg(mUrl)
                             .arg(fileId);
    putRequest(request, fileData, UPDATE_OVERLAY_FILE);
}

void
QSUSEStudio::updateOverlayFileMetadata(const QString &fileId,
                                   const QString &name,
                                   const QString &path,
                                   const QString &owner,
                                   const QString &group,
                                   const QString &permissions,
                                   const QString &enabled)
{
    QString request = QString("%1/api/v1/user/files/%2")
                             .arg(mUrl)
                             .arg(fileId);
    QByteArray data;
    QString xml;
    // TODO build up the XML call for this
    xml += "<file>";
    xml += "<id>" + fileId + "</id>";
    if (name != "")
        xml += "<filename>" + name + "</filename>";
    if (path != "")
        xml += "<path>" + path + "</path>";
    if (owner != "")
        xml += "<owner>" + owner + "</owner>";
    if (group != "")
        xml += "<group>" + group + "</group>";
    if (permissions != "")
        xml += "<permissions>" + permissions + "</permissions>";
    if (enabled != "")
        xml += "<enabled>" + enabled + "</enabled>";
    xml += "</file>";
    data += xml;
    putRequest(request, data, UPDATE_OVERLAY_FILE_METADATA);
}

void
QSUSEStudio::deleteOverlayFile(const QString &fileId)
{
    QString request = QString("%1/api/v1/user/files/%2")
                             .arg(mUrl)
                             .arg(fileId);
    deleteRequest(request, DELETE_OVERLAY_FILE);
}


void
QSUSEStudio::listRunningBuilds(const QString &applianceId)
{
    QString request = QString("%1/api/v1/user/running_builds?appliance_id=%2").arg(mUrl).arg(applianceId);
    getRequest(request, LIST_RUNNING_BUILDS);
}

void
QSUSEStudio::startBuild(const QString &applianceId,
                        const QString &forceBuild,
                        const QString &version,
                        const QString &image_type)
{
    QString request = QString("%1/api/v1/user/running_builds?appliance_id=%2&force=%3&version=%4&image_type=%5")
                             .arg(mUrl)
                             .arg(applianceId)
                             .arg(forceBuild)
                             .arg(version)
                             .arg(image_type);
    QByteArray empty;
    postRequest(request, empty, START_BUILD);
}

void
QSUSEStudio::cancelBuild(const QString &buildId)
{
    QString request = QString("%1/api/v1/user/running_builds/%2")
                             .arg(mUrl)
                             .arg(buildId);
    deleteRequest(request, CANCEL_BUILD);
}


void
QSUSEStudio::getRunningBuild(const QString &buildId)
{
    QString request = QString("%1/api/v1/user/running_builds/%2").arg(mUrl).arg(buildId);
    getRequest(request, GET_RUNNING_BUILD);
}

void
QSUSEStudio::listCompletedBuilds(const QString &applianceId)
{
    QString request = QString("%1/api/v1/user/builds?appliance_id=%2").arg(mUrl).arg(applianceId);
    getRequest(request, LIST_COMPLETED_BUILDS);
}


void
QSUSEStudio::getCompletedBuild(const QString &buildId)
{
    QString request = QString("%1/api/v1/user/builds/%2").arg(mUrl).arg(buildId);
    getRequest(request, GET_COMPLETED_BUILD);
}

void
QSUSEStudio::deleteCompletedBuild(const QString &buildId)
{
    QString request = QString("%1/api/v1/user/builds/%2")
                             .arg(mUrl)
                             .arg(buildId);
    deleteRequest(request, DELETE_COMPLETED_BUILD);
}

void
QSUSEStudio::listUploadedRPMs(const QString &baseSystem)
{
    QString request = QString("%1/api/v1/user/rpms?base_system=%2").arg(mUrl).arg(baseSystem);
    getRequest(request, LIST_UPLOADED_RPMS);
}

void
QSUSEStudio::getUploadedRPMInfo(const QString &rpmId)
{
    QString request = QString("%1/api/v1/user/rpms/%2").arg(mUrl).arg(rpmId);
    getRequest(request, GET_UPLOADED_RPM_INFO);
}

void
QSUSEStudio::getUploadedRPM(const QString &rpmId)
{
    QString request = QString("%1/api/v1/user/rpms/%2/data").arg(mUrl).arg(rpmId);
    getRequest(request, GET_UPLOADED_RPM);
}

void
QSUSEStudio::uploadRPM(const QString &baseSystem,
                       const QByteArray &rpm)
{
    QString request = QString("%1/api/v1/user/rpms?base_system=%2")
                             .arg(mUrl)
                             .arg(baseSystem);
    postRequest(request, rpm, UPLOAD_RPM);
}

void
QSUSEStudio::updateUploadedRPM(const QString &rpmId,
                               const QByteArray &rpm)
{
    QString request = QString("%1/api/v1/user/rpms/%2")
                             .arg(mUrl)
                             .arg(rpmId);
    putRequest(request, rpm, UPDATE_UPLOADED_RPM);
}

void
QSUSEStudio::deleteUploadedRPM(const QString &rpmId)
{
    QString request = QString("%1/api/v1/user/rpms/%2")
                             .arg(mUrl)
                             .arg(rpmId);
    deleteRequest(request, DELETE_UPLOADED_RPM);
}


void
QSUSEStudio::listRepositories(const QString &baseSystem, const QString &filter)
{
    QString request = QString("%1/api/v1/user/repositories?base_system=%2&filter=%3")
                             .arg(mUrl)
                             .arg(baseSystem)
                             .arg(filter);
    getRequest(request, LIST_REPOSITORIES);
}

void
QSUSEStudio::getRepository(const QString &repositoryId)
{
    QString request = QString("%1/api/v1/user/repositories/%2").arg(mUrl).arg(repositoryId);
    getRequest(request, GET_REPOSITORY);
}

void
QSUSEStudio::importRepository(const QString &url,
                              const QString &name)
{
    QString request = QString("%1/api/v1/user/repositories?url=%2&name=%3")
                             .arg(mUrl)
                             .arg(url)
                             .arg(name);
    QByteArray empty;
    postRequest(request, empty, IMPORT_REPOSITORY);

}

void
QSUSEStudio::printMap(const QVariantMap &map)
{
    dumpMap(map, 0);
}

/* Private functions */
void
QSUSEStudio::giveCredentials(QNetworkReply *reply,
                             QAuthenticator *authenticator)
{
    authenticator->setUser(mUsername);
    authenticator->setPassword(mApiKey);
}

void
QSUSEStudio::getRequest(const QString &url,
                        RequestType type)
{
    QNetworkRequest req = QNetworkRequest(QUrl(url));
    req.setAttribute(QNetworkRequest::User, type);
    QNetworkReply *reply = pManager->get(req);
    connect(reply, SIGNAL(finished()),
            this, SLOT(responseFinished()));
}

void
QSUSEStudio::postRequest(const QString &url,
                         const QByteArray &data,
                         RequestType type)
{
    QNetworkRequest req = QNetworkRequest(QUrl(url));
    req.setAttribute(QNetworkRequest::User, type);
    QNetworkReply *reply = pManager->post(req, data);
    connect(reply, SIGNAL(finished()),
            this, SLOT(responseFinished()));
}

void
QSUSEStudio::putRequest(const QString &url,
                        const QByteArray &data,
                        RequestType type)
{
    QNetworkRequest req = QNetworkRequest(QUrl(url));
    req.setAttribute(QNetworkRequest::User, type);
    QNetworkReply *reply = pManager->put(req, data);
    connect(reply, SIGNAL(finished()),
            this, SLOT(responseFinished()));
}

void
QSUSEStudio::deleteRequest(const QString &url,
                           RequestType type)
{
    QNetworkRequest req = QNetworkRequest(QUrl(url));
    req.setAttribute(QNetworkRequest::User, type);
    QNetworkReply *reply = pManager->deleteResource(req);
    connect(reply, SIGNAL(finished()),
            this, SLOT(responseFinished()));
}

void
QSUSEStudio::responseFinished()
{
    QVariantMap ret;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    int responseType = reply->request().attribute(QNetworkRequest::User).toInt();
    if ((responseType == GET_IMAGE_FILE) || (responseType == GET_OVERLAY_FILE) || (responseType == GET_UPLOADED_RPM))
    {
        QDataStream stream(reply);
        ret.insert("Stream", stream);
    }
    else
    {
        QString response = QString::fromUtf8((const char*) reply->readAll());
        ret = parseXml(response);
    }

    ret.insert("ResponseType", responseType);
    emit responseReady(ret);
}

/* This soooooooooooooo needs to be rewritten */
QVariantMap
QSUSEStudio::parseXml(const QString &xmlString)
{
    QVariantMap map;
    QXmlStreamReader xml(xmlString);
    xml.readNextStartElement();
    QString name = xml.name().toString();

    if (name == "account")
        map = parseAccountBlock(xml);
    else if (name == "template_sets")
        map = parseTemplateSets(xml);
    else if (name == "template_set")
        map = parseTemplateSet(xml);
    else if (name == "appliances")
        map = parseAppliances(xml);
    else if (name == "appliance")
        map = parseAppliance(xml);
    else if (name == "status")
        map = parseStatus(xml);
    else if (name == "repositories")
        map = parseRepositories(xml);
    else if (name == "repository")
        map = parseRepository(xml);
    else if (name == "software")
        map = parseSoftware(xml);
    else if (name == "software_map")
        map = parseSoftwareMap(xml);
    else if (name == "success")
        map = parseSuccess(xml);
    else if (name == "error")
        map = parseError(xml);
    else if (name == "gpg_keys")
        map = parseGPGKeys(xml);
    else if (name == "gpg_key")
        map = parseGPGKey(xml);
    else if (name == "files")
        map = parseFiles(xml);
    else if (name == "file")
        map = parseFile(xml);
    else if (name == "running_builds")
        map = parseRunningBuilds(xml);
    else if (name == "running_build")
        map = parseBlock(xml);
    else if (name == "builds")
        map = parseBuilds(xml);
    else if (name == "build")
        map = parseBlock(xml);
    else if (name == "rpms")
        map = parseRPMs(xml);
    else if (name == "rpm")
        map = parseBlock(xml);
    else
        qDebug() << xmlString;

    return(map);
}

QVariantMap
QSUSEStudio::parseAccountBlock(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList idList;
    QString name = "";
    QXmlStreamAttributes attr = xml.attributes();

    for (int i = 0; i < attr.size(); ++i)
        ret.insert(attr.at(i).name().toString(), attr.at(i).value().toString());

    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "account")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if ((name == "openid_urls") || (name == "disk_quota"))
                xml.readNext();
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (name != "")
            {
                if (name == "openid_url")
                    idList << xml.text().toString();
                else
                    ret.insert(name, xml.text().toString());
            }
        }

        xml.readNext();
    }
    if (!idList.empty())
        ret.insert("openids", idList);
    return(ret);
}

QVariantMap
QSUSEStudio::parseTemplateSets(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList templateList;
    QString name = "";
    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "template_sets")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if (name == "template_set")
                templateList << parseTemplateSet(xml);
        }
        xml.readNext();
    }
    if (!templateList.empty())
        ret.insert("template_sets", templateList);
    return(ret);
}

QVariantMap
QSUSEStudio::parseTemplateSet(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList templateList;
    QString name = "";

    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "template_set")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if (name == "template")
                templateList << parseBlock(xml);
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (name != "")
                ret.insert(name, xml.text().toString());
        }

        xml.readNext();
    }
    if (!templateList.empty())
        ret.insert("templates", templateList);
    return(ret);
}

QVariantMap
QSUSEStudio::parseAppliances(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList applianceList;
    QString name = "";
    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "appliances")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if (name == "appliance")
                applianceList << parseAppliance(xml);
        }
        xml.readNext();
    }
    if (!applianceList.empty())
        ret.insert("appliances", applianceList);
    return(ret);
}

QVariantMap
QSUSEStudio::parseAppliance(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList buildList;
    QString name = "";

    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "appliance")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if (name == "parent")
                ret.insert("parent", parseBlock(xml));
            else if (name == "build")
                buildList << parseBlock(xml);
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (name != "")
                ret.insert(name, xml.text().toString());
        }

        xml.readNext();
    }

    if (!buildList.empty())
        ret.insert("builds", buildList);
    return(ret);

}

QVariantMap
QSUSEStudio::parseStatus(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList issueList;
    QString name = "";

    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "status")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if (name == "issue")
                issueList << parseIssue(xml);
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (name != "")
                ret.insert(name, xml.text().toString());
        }

        xml.readNext();
    }

    if (!issueList.empty())
        ret.insert("issues", issueList);
    return(ret);
}

QVariantMap
QSUSEStudio::parseIssue(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QString name = "";

    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "issue")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if (name == "solution")
                ret.insert("solution", parseBlock(xml));
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (name != "")
                ret.insert(name, xml.text().toString());
        }

        xml.readNext();
    }

    return(ret);
}

QVariantMap
QSUSEStudio::parseRepositories(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList repoList;
    QString name = "";
    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "repositories")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if (name == "repository")
                repoList << parseRepository(xml);
        }
        xml.readNext();
    }
    if (!repoList.empty())
        ret.insert("repositories", repoList);
    return(ret);
}

QVariantMap
QSUSEStudio::parseRepository(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList matchList;
    QString name = "";
    QXmlStreamAttributes attr = xml.attributes();
    for (int i = 0; i < attr.size(); ++i)
        ret.insert(attr.at(i).name().toString(), attr.at(i).value().toString());

    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "repository")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if (name == "matches")
            {
                xml.readNext();
                matchList << parseBlock(xml);
            }
            else if (name == "software")
            {
                QVariantMap tmp = parseSoftware(xml);
                if (tmp.contains("software"))
                    ret.insert("software", tmp["software"]);
            }
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (name != "")
                ret.insert(name, xml.text().toString());
        }

        xml.readNext();
    }

    if (!matchList.empty())
        ret.insert("matches", matchList);
    return(ret);

}

QVariantMap
QSUSEStudio::parseSoftware(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList softwareList;
    QXmlStreamAttributes attr = xml.attributes();
    QString name = "";

    for (int i = 0; i < attr.size(); ++i)
        ret.insert(attr.at(i).name().toString(), attr.at(i).value().toString());

    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "software")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if ((name == "package") || (name == "pattern"))
            {
                QVariantMap pkgMap;
                QXmlStreamAttributes attr = xml.attributes();
                for (int i = 0; i < attr.size(); ++i)
                    pkgMap.insert(attr.at(i).name().toString(), attr.at(i).value().toString());

                xml.readNext();
                pkgMap.insert(name, xml.text().toString());
                softwareList << pkgMap;
                xml.readNext();
            }
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (name != "")
                ret.insert(name, xml.text().toString());
        }
        xml.readNext();
    }

    if (!softwareList.empty())
        ret.insert("software", softwareList);
    return(ret);
}

QVariantMap
QSUSEStudio::parseSoftwareMap(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList repoList;
    QXmlStreamAttributes attr = xml.attributes();
    QString name = "";

    for (int i = 0; i < attr.size(); ++i)
        ret.insert(attr.at(i).name().toString(), attr.at(i).value().toString());

    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "software_map")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if (name == "repository")
                repoList << parseRepository(xml);
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (name != "")
                ret.insert(name, xml.text().toString());
        }
        xml.readNext();
    }

    if (!repoList.empty())
        ret.insert("repositories", repoList);
    return(ret);
}

QVariantMap
QSUSEStudio::parseRunningBuilds(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList buildList;
    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "running_builds")
                break;
        }
        else if (xml.isStartElement())
        {
            if (xml.name().toString() == "running_build")
                buildList << parseBlock(xml);
        }
        xml.readNext();
    }
    if (!buildList.empty())
        ret.insert("running_builds", buildList);
    return(ret);
}

QVariantMap
QSUSEStudio::parseRPMs(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList rpmList;
    QXmlStreamAttributes attr = xml.attributes();

    for (int i = 0; i < attr.size(); ++i)
        ret.insert(attr.at(i).name().toString(), attr.at(i).value().toString());

    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "rpms")
                break;
        }
        else if (xml.isStartElement())
        {
            if (xml.name().toString() == "rpm")
                rpmList << parseBlock(xml);
        }
        xml.readNext();
    }
    if (!rpmList.empty())
        ret.insert("rpms", rpmList);
    return(ret);
}

QVariantMap
QSUSEStudio::parseBuilds(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList buildList;
    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "builds")
                break;
        }
        else if (xml.isStartElement())
        {
            if (xml.name().toString() == "builds")
                buildList << parseBlock(xml);
        }
        xml.readNext();
    }
    if (!buildList.empty())
        ret.insert("builds", buildList);
    return(ret);
}

QVariantMap
QSUSEStudio::parseSuccess(QXmlStreamReader &xml)
{
    QVariantMap ret;
    while (!xml.atEnd())
    {
        if (xml.isStartElement())
        {
            if (xml.name().toString() == "status")
                ret = parseStatus(xml);
        }
        xml.readNext();
    }
    ret.insert("status", "success");
    return(ret);
}

QVariantMap
QSUSEStudio::parseError(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QString name = "";
    ret.insert("status", "error");
    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "error")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (name != "")
                ret.insert(name, xml.text().toString());
        }
        xml.readNext();
    }
    return(ret);
}

QVariantMap
QSUSEStudio::parseFiles(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList fileList;
    QString name = "";

    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "files")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if (name == "file")
                fileList << parseFile(xml);
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (name != "")
                ret.insert(name, xml.text().toString());
        }
        xml.readNext();
    }

    if (!fileList.empty())
        ret.insert("files", fileList);
    return(ret);
}

QVariantMap
QSUSEStudio::parseFile(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QString name = "";
    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "file")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (name != "")
                ret.insert(name, xml.text().toString());
        }
        xml.readNext();
    }
    return(ret);
}

QVariantMap
QSUSEStudio::parseGPGKeys(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QVariantList keyList;
    QString name = "";
    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == "gpg_keys")
                break;
        }
        else if (xml.isStartElement())
        {
            name = xml.name().toString();
            if (name == "gpg_key")
                keyList << parseGPGKey(xml);
        }
        xml.readNext();
    }
    if (!keyList.empty())
        ret.insert("keys", keyList);

    return(ret);
}

QVariantMap
QSUSEStudio::parseGPGKey(QXmlStreamReader &xml)
{
    return (parseBlock(xml));
}

QVariantMap
QSUSEStudio::parseBlock(QXmlStreamReader &xml)
{
    QVariantMap ret;
    QString blockName = "";
    QString curName = "";
    QXmlStreamAttributes attr = xml.attributes();

    for (int i = 0; i < attr.size(); ++i)
        ret.insert(attr.at(i).name().toString(), attr.at(i).value().toString());

    blockName = xml.name().toString();
    xml.readNext();
    while (!xml.atEnd())
    {
        if (xml.isEndElement())
        {
            if (xml.name().toString() == blockName)
                break;
        }
        else if (xml.isStartElement())
        {
            curName = xml.name().toString();
        }
        else if (xml.isCharacters() && !xml.isWhitespace())
        {
            if (curName != "")
                ret.insert(curName, xml.text().toString());
        }
        xml.readNext();
    }
    return(ret);
}
/* Pretty-print the QVariantMaps */

void
QSUSEStudio::dumpMap(const QVariantMap &map, int indentLevel)
{
    QMapIterator<QString, QVariant> i(map);
    if (indentLevel != 0)
    {
        printIndent(indentLevel);
        printf("----[ Map ]----\n");
    }
    while (i.hasNext())
    {
        i.next();
        printIndent(indentLevel);
        printf("%s --> ", qPrintable(i.key()));
        switch (i.value().type())
        {
            case QVariant::String:
            {
                printf("%s\n", qPrintable(i.value().toString()));
                break;
            }
            case QVariant::Map:
            {
                printf("[Map]\n");
                dumpMap(i.value().toMap(), indentLevel + 1);
                break;
            }
            case QVariant::List:
            {
                printf("[List]\n");
                dumpList(i.value().toList(), indentLevel + 1);
                break;
            }
            case QVariant::Int:
            {
                printf("%i\n", i.value().toInt());
                break;
            }
            default:
                printf("Unknown\n");
                break;
        }
    }
    if (indentLevel != 0)
    {
        printIndent(indentLevel);
        printf("---------------\n\n");
    }

}

void
QSUSEStudio::dumpList(const QVariantList &list, int indentLevel)
{

    for (int i = 0; i < list.size(); ++i)
    {
        switch (list.at(i).type())
        {
            case QVariant::String:
            {
                printIndent(indentLevel);
                printf("%s\n", qPrintable(list.at(i).toString()));
                break;
            }
            case QVariant::Map:
            {
                dumpMap(list.at(i).toMap(), indentLevel + 1);
                break;
            }
            case QVariant::List:
            {
                printIndent(indentLevel);
                printf("[List]\n");
                dumpList(list.at(i).toList(), indentLevel + 1);
                break;
            }
            default:
                printf("Unknown\n");
                break;
        }
    }
}

void
QSUSEStudio::printIndent(int indentLevel)
{
    if (indentLevel == 0) return;
    QString justify = " ";
    printf("%s",qPrintable(justify.rightJustified((indentLevel * 5), ' ')));
}

#include "qsusestudio.moc"
