/*
 *   Copyright 2008 Chani Armitage <chani@kde.org>
 *   Copyright 2008, 2009 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2010 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef APPLETINTERFACE_H
#define APPLETINTERFACE_H

#include <QQuickItem>
#include <QScriptValue>
#include <QQuickView>

#include <Plasma/Applet>
#include <Plasma/Theme>

#include "declarativeappletscript.h"

class QAction;
class QmlAppletScript;
class QSignalMapper;
class QSizeF;

class ConfigPropertyMap;
class ConfigView;

class QmlObject;

namespace Plasma
{
    class ConfigLoader;
} // namespace Plasma

class AppletInterface : public QQuickItem
{
    Q_OBJECT

    /**
     * The QML root object defined in the applet main.qml will be direct child of an AppletInterface instance
     */

    /**
     * Version of the QML2 script engine
     */
    Q_PROPERTY(int apiVersion READ apiVersion CONSTANT)

    /**
     * User friendly title for the plasmoid: it's the localized applet name by default
     */
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

    //TODO: writable icon
    /**
     * Icon to represent the plasmoid
     */
    Q_PROPERTY(QString icon READ icon CONSTANT)

    /**
     * Applet id: is unique in the whole Plasma session and will never change across restarts
     */
    Q_PROPERTY(uint id READ id CONSTANT)

    /**
     * FormFactor for the plasmoid
     */
    Q_PROPERTY(Plasma::Types::FormFactor formFactor READ formFactor NOTIFY formFactorChanged)

    /**
     * Location for the plasmoid
     */
    Q_PROPERTY(Plasma::Types::Location location READ location NOTIFY locationChanged)

    /**
     * Current activity name the plasmoid is in
     */
    Q_PROPERTY(QString currentActivity READ currentActivity NOTIFY contextChanged)

    /**
     * Configuration object: each config key will be a writable property of this object. property bindings work.
     */
    Q_PROPERTY(QObject* configuration READ configuration CONSTANT)

    /**
     * FIXME-API: do we still want this?
     * current active configuration
     */
    Q_PROPERTY(QString activeConfig WRITE setActiveConfig READ activeConfig)

    /**
     * When true the plasmoid is busy. The containment may graphically indicate that drawing for instance a spinner busy widget over it
     */
    Q_PROPERTY(bool busy WRITE setBusy READ isBusy NOTIFY busyChanged)

    /**
     * True when the applet is showing its full representation. either as the main only view, or in a popup.
     * Setting it will open or close the popup if the plasmoid is iconified, however it won't have effect if the applet is open
     */
    Q_PROPERTY(bool expanded WRITE setExpanded READ isExpanded NOTIFY expandedChanged)

    /**
     * How the applet wants its background to be drawn. The containment may chose to ignore this hint.
     */
    Q_PROPERTY(Plasma::Types::BackgroundHints backgroundHints WRITE setBackgroundHints READ backgroundHints NOTIFY backgroundHintsChanged)

    /**
     * The immutability state of the Corona. the plasmoid implementation should avoid allowing "dangerous" modifications from the user when in an immutable mode
     */
    Q_PROPERTY(bool immutable READ immutable NOTIFY immutableChanged)

    /**
     * True when the user is configuring, for instance when the configuration dialog is open.
     */
    Q_PROPERTY(bool userConfiguring READ userConfiguring NOTIFY userConfiguringChanged)

    /**
     * Status of the plasmoid: useful to instruct the shell if this plasmoid is requesting attention, if is accepting input, or if is in an idle, inactive state
     */
    Q_PROPERTY(Plasma::Types::ItemStatus status READ status WRITE setStatus NOTIFY statusChanged)

    /**
     * Sets the associated application of this plasmoid, if the plasmoid is representing the "compact" view for some application or for some document type. 
     * TODO: a way to set associated application urls.
     */
    Q_PROPERTY(QString associatedApplication WRITE setAssociatedApplication READ associatedApplication)

    // TODO: This was moved up from ContainmentInterface because it is required by the
    // Task Manager applet (for "Show only tasks from this screen") and no Qt API exposes
    // screen numbering. An alternate solution that doesn't extend the applet interface
    // would be preferrable if found.
    Q_PROPERTY(int screen READ screen NOTIFY screenChanged)

    //Size hints. Note that the containments may chose to not respect them.
    Q_PROPERTY(qreal minimumWidth READ minimumWidth NOTIFY minimumWidthChanged)
    Q_PROPERTY(qreal minimumHeight READ minimumHeight NOTIFY minimumHeightChanged)
    Q_PROPERTY(qreal maximumWidth READ maximumWidth NOTIFY maximumWidthChanged)
    Q_PROPERTY(qreal maximumHeight READ maximumHeight NOTIFY maximumHeightChanged)
    //FIXME?implicitWidth/height is already there
    Q_PROPERTY(qreal implicitWidth READ implicitWidth NOTIFY implicitWidthChanged)
    Q_PROPERTY(qreal implicitHeight READ implicitHeight NOTIFY implicitHeightChanged)

    /**
     * If the plasmoid is in a linear layout, such as a panel, it indicates to take as much horizontal space as possible
     */
    Q_PROPERTY(bool fillWidth READ fillWidth NOTIFY fillWidthChanged)

    /**
     * If the plasmoid is in a linear layout, such as a panel, it indicates to take as much vertical space as possible
     */
    Q_PROPERTY(bool fillHeight READ fillHeight NOTIFY fillHeightChanged)

public:
    AppletInterface(DeclarativeAppletScript *script, QQuickItem *parent = 0);
    ~AppletInterface();

//API not intended for the QML part
    QmlObject *qmlObject();

    QList<QAction*> contextualActions() const;

    inline Plasma::Applet *applet() const { return m_appletScriptEngine->applet(); }

//QML API-------------------------------------------------------------------

    Q_INVOKABLE void setConfigurationRequired(bool needsConfiguring, const QString &reason = QString());

    Q_INVOKABLE void setActionSeparator(const QString &name);
    Q_INVOKABLE void setAction(const QString &name, const QString &text,
                               const QString &icon = QString(), const QString &shortcut = QString());

    Q_INVOKABLE void removeAction(const QString &name);

    Q_INVOKABLE QAction *action(QString name) const;

    Q_INVOKABLE QVariant readConfig(const QString &entry) const;

    Q_INVOKABLE void writeConfig(const QString &entry, const QVariant &value);

    Q_INVOKABLE QString file(const QString &fileType);
    Q_INVOKABLE QString file(const QString &fileType, const QString &filePath);

    Q_INVOKABLE void debug(const QString &msg);

    //FIXME-API: remove?
    Q_INVOKABLE QString downloadPath(const QString &file);
    Q_INVOKABLE QStringList downloadedFiles() const;


//PROPERTY ACCESSORS-------------------------------------------------------------------
    QString icon() const;

    QString title() const;
    void setTitle(const QString &title);

    uint id() const;

    Plasma::Types::FormFactor formFactor() const;

    Plasma::Types::Location location() const;

    QString currentActivity() const;

    QObject* configuration() const;

    bool isBusy() const;
    void setBusy(bool busy);

    bool isExpanded() const;
    void setExpanded(bool expanded);

    Plasma::Types::BackgroundHints backgroundHints() const;
    void setBackgroundHints(Plasma::Types::BackgroundHints hint);

    void setAssociatedApplication(const QString &string);
    QString associatedApplication() const;

    void setStatus(const Plasma::Types::ItemStatus &status);
    Plasma::Types::ItemStatus status() const;

    int screen() const;

    QString activeConfig() const;
    void setActiveConfig(const QString &name);

    bool immutable() const;
    bool userConfiguring() const;
    int apiVersion() const;

    bool fillWidth() const;
    bool fillHeight() const;
    qreal minimumWidth() const;
    qreal minimumHeight() const;
    qreal maximumWidth() const;
    qreal maximumHeight() const;
    qreal implicitWidth() const;
    qreal implicitHeight() const;

Q_SIGNALS:
    void releaseVisualFocus();
    void configNeedsSaving();

//PROPERTY change notifiers--------------
    void titleChanged();
    void formFactorChanged();
    void locationChanged();
    void contextChanged();
    void immutableChanged();
    void statusChanged();
    void backgroundHintsChanged();
    void busyChanged();
    void expandedChanged();
    void screenChanged();

    void minimumWidthChanged();
    void minimumHeightChanged();
    void maximumWidthChanged();
    void maximumHeightChanged();
    void implicitWidthChanged();
    void implicitHeightChanged();
    void fillWidthChanged();
    void fillHeightChanged();
    void userConfiguringChanged();

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void itemChange(ItemChange change, const ItemChangeData &value);

    DeclarativeAppletScript *m_appletScriptEngine;

protected Q_SLOTS:
    virtual void init();

private Q_SLOTS:
    void compactRepresentationCheck();

private:
    //Helper for minimumWidth etc.
    qreal readGraphicsObjectSizeHint(const char *hint) const;

    QStringList m_actions;
    QSignalMapper *m_actionSignals;
    QString m_currentConfig;
    QMap<QString, Plasma::ConfigLoader*> m_configs;


    ConfigPropertyMap *m_configuration;

//UI-specific members ------------------
    QmlObject *m_qmlObject;
    QWeakPointer<QObject> m_compactUiObject;

    QTimer *m_collapseTimer;

    Plasma::Types::BackgroundHints m_backgroundHints;
    bool m_busy : 1;
    bool m_expanded : 1;
    friend class ContainmentInterface;
};

#endif
