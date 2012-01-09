/*
 *   Copyright 2011 by Marco Martin <mart@kde.org>

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

#include "plasmacomponentsplugin.h"

#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeItem>

#include "qrangemodel.h"

#include <KSharedConfig>
#include <KDebug>

#include "enums.h"
#include "qmenu.h"
#include "qmenuitem.h"
#include "kdialogproxy.h"
#include "fullscreendialog.h"

Q_EXPORT_PLUGIN2(plasmacomponentsplugin, PlasmaComponentsPlugin)

class BKSingleton
{
public:
   EngineBookKeeping self;
};
K_GLOBAL_STATIC(BKSingleton, privateBKSelf)

EngineBookKeeping::EngineBookKeeping()
{
}

EngineBookKeeping *EngineBookKeeping::self()
{
    return &privateBKSelf->self;
}

QDeclarativeEngine *EngineBookKeeping::engineFor(QObject *item) const
{return m_engines.values().first();
    foreach (QDeclarativeEngine *engine, m_engines) {
        QObject *root = engine->rootContext()->contextObject();
        QObject *candidate = item;
        while (candidate) {
            if (candidate == root) {
                return engine;
            }
            candidate = candidate->parent();
        }
    }
    return 0;
}

void EngineBookKeeping::insertEngine(QDeclarativeEngine *engine)
{
    m_engines.insert(engine);
}



void PlasmaComponentsPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    QDeclarativeExtensionPlugin::initializeEngine(engine, uri);
    EngineBookKeeping::self()->insertEngine(engine);
}

void PlasmaComponentsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.components"));

    QString componentsPlatform = getenv("KDE_PLASMA_COMPONENTS_PLATFORM");
    if (componentsPlatform.isEmpty()) {
        KConfigGroup cg(KSharedConfig::openConfig("kdeclarativerc"), "Components-platform");
        componentsPlatform = cg.readEntry("name", "desktop");
    }

    if (componentsPlatform == "desktop") {
        qmlRegisterType<KDialogProxy>(uri, 0, 1, "QueryDialog");

        qmlRegisterType<QMenuProxy>(uri, 0, 1, "Menu");
        qmlRegisterType<QMenuItem>(uri, 0, 1, "MenuItem");
    }

    qmlRegisterType<FullScreenDialog>(uri, 0, 1, "FullScreenDialog");
    qmlRegisterType<Plasma::QRangeModel>(uri, 0, 1, "RangeModel");

    qmlRegisterUncreatableType<DialogStatus>(uri, 0, 1, "DialogStatus", "");
    qmlRegisterUncreatableType<PageOrientation>(uri, 0, 1, "PageOrientation", "");
    qmlRegisterUncreatableType<PageStatus>(uri, 0, 1, "PageStatus", "");
}


#include "plasmacomponentsplugin.moc"

