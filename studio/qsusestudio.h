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


#ifndef QSUSESTUDIO_HPP
#define QSUSESTUDIO_HPP

#include "qsusestudio_global.h"
#include <QObject>
#include <QVariantMap>
#include <QVariantList>
#include <QXmlStreamReader>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QStringList>

#define PUBLIC_STUDIO_URL "http://susestudio.com"

class QSUSESTUDIOSHARED_EXPORT QSUSEStudio : public QObject
{
Q_OBJECT
public:
enum RequestType
{
    GET_ACCOUNT_INFO,
    GET_TEMPLATE_SETS,
    CREATE_APPLIANCE,
    DELETE_APPLIANCE,
    LIST_APPLIANCES,
    GET_APPLIANCE_DETAILS,
    GET_APPLIANCE_STATUS,
    LIST_APPLIANCE_REPOSITORIES,
    SET_APPLIANCE_REPOSITORIES,
    ADD_APPLIANCE_REPOSITORY,
    REMOVE_APPLIANCE_REPOSITORY,
    LIST_APPLIANCE_SOFTWARE,
    UPDATE_APPLIANCE_SOFTWARE,
    ADD_APPLIANCE_SOFTWARE,
    ADD_APPLIANCE_PATTERN,
    REMOVE_APPLIANCE_SOFTWARE,
    REMOVE_APPLIANCE_PATTERN,
    LIST_APPLIANCE_INSTALLED_SOFTWARE,
    SEARCH_SOFTWARE,
    BAN_APPLIANCE_SOFTWARE,
    UNBAN_APPLIANCE_SOFTWARE,
    GET_IMAGE_FILE,
    LIST_APPLIANCE_GPG_KEYS,
    ADD_APPLIANCE_GPG_KEY,
    REMOVE_APPLIANCE_GPG_KEY,
    LIST_OVERLAY_FILES,
    ADD_OVERLAY_FILE,
    UPDATE_OVERLAY_FILE,
    UPDATE_OVERLAY_FILE_METADATA,
    DELETE_OVERLAY_FILE,
    GET_OVERLAY_FILE,
    GET_OVERLAY_FILE_METADATA,
    LIST_RUNNING_BUILDS,
    GET_RUNNING_BUILD,
    START_BUILD,
    CANCEL_BUILD,
    LIST_COMPLETED_BUILDS,
    GET_COMPLETED_BUILD,
    DELETE_COMPLETED_BUILD,
    LIST_UPLOADED_RPMS,
    GET_UPLOADED_RPM,
    GET_UPLOADED_RPM_INFO,
    UPLOAD_RPM,
    UPDATE_UPLOADED_RPM,
    DELETE_UPLOADED_RPM,
    LIST_REPOSITORIES,
    GET_REPOSITORY,
    IMPORT_REPOSITORY
};

    QSUSEStudio(const QString &username,
                const QString &key,
                const QString &url = PUBLIC_STUDIO_URL);
    ~QSUSEStudio();

    /**
     * All public functions return QVariantMaps.
     * The structure of each is mapped almost directly to
     * the Studio API documentation.  The objects
     * within the maps will either be QStrings or QVariantLists
     * which in turn act as arrays of QVariantMaps or QStrings.
     */

    void getAccountInfo();

    /* Template sets */

    void getTemplateSets();
    void getTemplateSets(const QString &name);

    /* Appliances */
    void createAppliance(const QString &templateId,
                         const QString &name = "",
                         const QString &arch = "");
    void listAppliances();
    void getApplianceDetails(const QString &id);
    void getApplianceStatus(const QString &id);
    void deleteAppliance(const QString &applianceId);

    //// Appliance repositories ////
    void listApplianceRepositories(const QString &applianceId);
    void setApplianceRepositories(const QString &applianceId,
                                  const QStringList &repositoryList);
    void addApplianceRepository(const QString &applianceId,
                                const QString &repositoryId);
    void removeApplianceRepository(const QString &applianceId,
                                   const QString &repositoryId);
    void addApplianceUserRepository(const QString &applianceId);

    //// Appliance software selection ////
    void listApplianceSoftware(const QString &id);
    void listInstalledSoftware(const QString &id, const QString &buildId = "");
    void searchSoftware(const QString &id, const QString &query, bool allFields = false, bool allRepos = false);
    void updateApplianceSoftware(const QString &applianceId);
    void addApplianceSoftware(const QString &applianceId,
                              const QString &packageName,
                              const QString &packageVersion = "",
                              const QString &packageRepositoryId = "");
    void addAppliancePattern(const QString &applianceId,
                             const QString &patternName,
                             const QString &patternVersion = "",
                             const QString &patternRepositoryId = "");
    void removeApplianceSoftware(const QString &applianceId,
                                 const QString &packageName);
    void removeAppliancePattern(const QString &applianceId,
                                const QString &patternName);
    void banApplianceSoftware(const QString &applianceId,
                              const QString &packageName);
    void unbanApplianceSoftware(const QString &applianceId,
                                const QString &packageName);

    //// Appliance image files ////
    void getImageFile(const QString &id, const QString &buildId, const QString &path);

    //// Appliance GPG keys ////
    /* This will also get details for the GPG key if keyId is specified */
    void listApplianceGPGKeys(const QString &id, const QString &keyId = "");
    void addApplianceGPGKey(const QString &applianceId,
                            const QString &keyName,
                            const QString &key,
                            const QString &target = "rpm");
    void removeApplianceGPGKey(const QString &applianceId,
                               const QString &keyId);

    //// Overlay files ////
    void listOverlayFiles(const QString &id);
    void getOverlayFile(const QString &id);
    void getOverlayFileMetadata(const QString &id);
    void addOverlayFile(const QString &applianceId,
                        const QByteArray &fileData,
                        const QString &name = "",
                        const QString &path = "",
                        const QString &owner = "",
                        const QString &group = "",
                        const QString &permissions = "",
                        const QString &enabled = "");

    void updateOverlayFile(const QString &fileId,
                           const QByteArray &fileData);
    void updateOverlayFileMetadata(const QString &fileId,
                                   const QString &name = "",
                                   const QString &path = "",
                                   const QString &owner = "",
                                   const QString &group = "",
                                   const QString &permissions = "",
                                   const QString &enabled = "true");
    void deleteOverlayFile(const QString &fileId);


    //// Running builds ////
    void listRunningBuilds(const QString &applianceId);
    void getRunningBuild(const QString &buildId);
    void startBuild(const QString &applianceId,
                    const QString &forceBuild = "",
                    const QString &version = "",
                    const QString &image_type = "");
    void cancelBuild(const QString &buildId);

    //// Completed builds ////
    void listCompletedBuilds(const QString &applianceId);
    void getCompletedBuild(const QString &buildId);
    void deleteCompletedBuild(const QString &buildId);

    //// Uploaded rpms ////
    void listUploadedRPMs(const QString &baseSystem);
    void getUploadedRPMInfo(const QString &rpmId);
    void getUploadedRPM(const QString &rpmId);
    void uploadRPM(const QString &baseSystem,
                   const QByteArray &rpm);
    void updateUploadedRPM(const QString &rpmId,
                           const QByteArray &rpm);
    void deleteUploadedRPM(const QString &rpmId);

    //// System-wide repositories ////
    void listRepositories(const QString &baseSystem = "", const QString &filter = "");
    void getRepository(const QString &repositoryId);
    void importRepository(const QString &url,
                          const QString &name);

    /* Debug function */
    static void printMap(const QVariantMap &map);

signals:
    void responseReady(QVariantMap response); // Emitted for the user of the API

public slots:
    void responseFinished();
    void giveCredentials(QNetworkReply *reply, QAuthenticator *authenticator);

private:
    static void dumpMap(const QVariantMap &map, int indentLevel = 0);
    static void dumpList(const QVariantList &list, int indentLevel = 0);
    static void printIndent(int indentLevel);

    void getRequest(const QString &url,
                    RequestType type);
    void postRequest(const QString &url,
                     const QByteArray &data,
                     RequestType type);
    void putRequest(const QString &url,
                    const QByteArray &data,
                    RequestType type);
    void deleteRequest(const QString &url,
                       RequestType type);
    QVariantMap parseXml(const QString &xmlString);
    QVariantMap parseBlock(QXmlStreamReader &xml);

    /* TODO: Fold these into far fewer functions */
    QVariantMap parseAccountBlock(QXmlStreamReader &xml);
    QVariantMap parseTemplateSets(QXmlStreamReader &xml);
    QVariantMap parseTemplateSet(QXmlStreamReader &xml);
    QVariantMap parseAppliances(QXmlStreamReader &xml);
    QVariantMap parseAppliance(QXmlStreamReader &xml);
    QVariantMap parseStatus(QXmlStreamReader &xml);
    QVariantMap parseIssue(QXmlStreamReader &xml);
    QVariantMap parseRepositories(QXmlStreamReader &xml);
    QVariantMap parseRepository(QXmlStreamReader &xml);
    QVariantMap parseSoftware(QXmlStreamReader &xml);
    QVariantMap parseSoftwareMap(QXmlStreamReader &xml);
    QVariantMap parseSuccess(QXmlStreamReader &xml);
    QVariantMap parseError(QXmlStreamReader &xml);
    QVariantMap parseGPGKeys(QXmlStreamReader &xml);
    QVariantMap parseGPGKey(QXmlStreamReader &xml);
    QVariantMap parseFiles(QXmlStreamReader &xml);
    QVariantMap parseFile(QXmlStreamReader &xml);
    QVariantMap parseRunningBuilds(QXmlStreamReader &xml);
    QVariantMap parseBuilds(QXmlStreamReader &xml);
    QVariantMap parseRPMs(QXmlStreamReader &xml);

    QList<QString> listItems;
    QList<QString> blockItems;
    QString mApiKey;
    QString mUsername;
    QString mUrl;
    QNetworkAccessManager *pManager;
};
#endif
