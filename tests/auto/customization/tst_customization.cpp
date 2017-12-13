/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/qtest.h>
#include <QtCore/private/qhooks_p.h>
#include <QtCore/qregularexpression.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickwindow.h>
#include <QtQuickControls2/qquickstyle.h>
#include "../shared/visualtestutil.h"

using namespace QQuickVisualTestUtil;

class tst_customization : public QQmlDataTest
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void creation_data();
    void creation();

    void comboPopup();

private:
    void reset();
    void addHooks();
    void removeHooks();

    QObject* createControl(const QString &type);

    QQmlEngine *engine = nullptr;
};

Q_GLOBAL_STATIC(QStringList, qt_createdQObjects)
Q_GLOBAL_STATIC(QStringList, qt_destroyedQObjects)

extern "C" Q_DECL_EXPORT void qt_addQObject(QObject *object)
{
    // objectName is not set at construction time
    QObject::connect(object, &QObject::objectNameChanged, [object](const QString &objectName) {
        if (!objectName.isEmpty())
            qt_createdQObjects()->append(objectName);
    });
}

extern "C" Q_DECL_EXPORT void qt_removeQObject(QObject *object)
{
    QString objectName = object->objectName();
    if (!objectName.isEmpty())
        qt_destroyedQObjects()->append(objectName);
}

void tst_customization::init()
{
    engine = new QQmlEngine(this);

    qtHookData[QHooks::AddQObject] = reinterpret_cast<quintptr>(&qt_addQObject);
    qtHookData[QHooks::RemoveQObject] = reinterpret_cast<quintptr>(&qt_removeQObject);
}

void tst_customization::cleanup()
{
    qtHookData[QHooks::AddQObject] = 0;
    qtHookData[QHooks::RemoveQObject] = 0;

    delete engine;
    engine = nullptr;

    qmlClearTypeRegistrations();

    reset();
}

void tst_customization::reset()
{
    qt_createdQObjects()->clear();
    qt_destroyedQObjects()->clear();
}

QObject* tst_customization::createControl(const QString &name)
{
    QQmlComponent component(engine);
    component.setData("import QtQuick.Controls 2.2; " + name.toUtf8() + " { }", QUrl());
    QObject *obj = component.create();
    if (!obj)
        qDebug() << component.errorString();
    return obj;
}

void tst_customization::creation_data()
{
    QTest::addColumn<QString>("style");
    QTest::addColumn<QString>("type");
    QTest::addColumn<QStringList>("delegates");

    // the "empty" style does not contain any delegates
    QTest::newRow("empty:ApplicationWindow") << "empty" << "ApplicationWindow"<< (QStringList() << "applicationwindow-empty");
    QTest::newRow("empty:Button") << "empty" << "Button"<< (QStringList() << "button-empty");
    QTest::newRow("empty:CheckBox") << "empty" << "CheckBox" << (QStringList() << "checkbox-empty");
    QTest::newRow("empty:ComboBox") << "empty" << "ComboBox" << (QStringList() << "combobox-empty");
    QTest::newRow("empty:Dial") << "empty" << "Dial" << (QStringList() << "dial-empty");
    QTest::newRow("empty:Label") << "empty" << "Label"<< (QStringList() << "label-empty");
    QTest::newRow("empty:RadioButton") << "empty" << "RadioButton" << (QStringList() << "radiobutton-empty");
    QTest::newRow("empty:RangeSlider") << "empty" << "RangeSlider" << (QStringList() << "rangeslider-empty");
    QTest::newRow("empty:Slider") << "empty" << "Slider" << (QStringList() << "slider-empty");
    QTest::newRow("empty:TextField") << "empty" << "TextField"<< (QStringList() << "textfield-empty");
    QTest::newRow("empty:TextArea") << "empty" << "TextArea"<< (QStringList() << "textarea-empty");

    // the "incomplete" style is missing most delegates
    QTest::newRow("incomplete:Button") << "incomplete" << "Button" << (QStringList() << "button-incomplete" << "button-background-incomplete");
    QTest::newRow("incomplete:CheckBox") << "incomplete" << "CheckBox" << (QStringList() << "checkbox-incomplete" << "checkbox-contentItem-incomplete");
    QTest::newRow("incomplete:ComboBox") << "incomplete" << "ComboBox" << (QStringList() << "combobox-incomplete" << "combobox-contentItem-incomplete");
    QTest::newRow("incomplete:Dial") << "incomplete" << "Dial" << (QStringList() << "dial-incomplete" << "dial-handle-incomplete");
    QTest::newRow("incomplete:RadioButton") << "incomplete" << "RadioButton" << (QStringList() << "radiobutton-incomplete" << "radiobutton-indicator-incomplete");
    QTest::newRow("incomplete:RangeSlider") << "incomplete" << "RangeSlider" << (QStringList() << "rangeslider-incomplete" << "rangeslider-first-handle-incomplete" << "rangeslider-second-handle-incomplete");
    QTest::newRow("incomplete:Slider") << "incomplete" << "Slider" << (QStringList() << "slider-incomplete" << "slider-handle-incomplete");

    // the "simple" style simulates a proper style and contains most delegates
    QTest::newRow("simple:ApplicationWindow") << "simple" << "ApplicationWindow" << (QStringList() << "applicationwindow-simple" << "applicationwindow-background-simple");
    QTest::newRow("simple:Button") << "simple" << "Button" << (QStringList() << "button-simple" << "button-background-simple" << "button-contentItem-simple");
    QTest::newRow("simple:CheckBox") << "simple" << "CheckBox" << (QStringList() << "checkbox-simple" << "checkbox-contentItem-simple" << "checkbox-indicator-simple");
    QTest::newRow("simple:ComboBox") << "simple" << "ComboBox" << (QStringList() << "combobox-simple" << "combobox-background-simple" << "combobox-contentItem-simple" << "combobox-indicator-simple");
    QTest::newRow("simple:Dial") << "simple" << "Dial" << (QStringList() << "dial-simple" << "dial-background-simple" << "dial-handle-simple");
    QTest::newRow("simple:Label") << "simple" << "Label" << (QStringList() << "label-simple" << "label-background-simple");
    QTest::newRow("simple:RadioButton") << "simple" << "RadioButton" << (QStringList() << "radiobutton-simple" << "radiobutton-contentItem-simple" << "radiobutton-indicator-simple");
    QTest::newRow("simple:RangeSlider") << "simple" << "RangeSlider" << (QStringList() << "rangeslider-simple" << "rangeslider-background-simple" << "rangeslider-first-handle-simple" << "rangeslider-second-handle-simple");
    QTest::newRow("simple:Slider") << "simple" << "Slider" << (QStringList() << "slider-simple" << "slider-background-simple" << "slider-handle-simple");
    QTest::newRow("simple:TextField") << "simple" << "TextField" << (QStringList() << "textfield-simple" << "textfield-background-simple");
    QTest::newRow("simple:TextArea") << "simple" << "TextArea" << (QStringList() << "textarea-simple" << "textarea-background-simple");

    // the "override" style overrides various delegates in the above styles
    QTest::newRow("override:ApplicationWindow") << "override" << "ApplicationWindow" << (QStringList() << "applicationwindow-override" << "applicationwindow-background-override" << "applicationwindow-simple"); // overrides "simple"
    QTest::newRow("override:Button") << "override" << "Button" << (QStringList() << "button-override" << "button-background-override" << "button-contentItem-override" << "button-empty"); // overrides "empty"
    QTest::newRow("override:CheckBox") << "override" << "CheckBox" << (QStringList() << "checkbox-override" << "checkbox-background-override" << "checkbox-contentItem-incomplete" << "checkbox-incomplete"); // overrides "incomplete"
    QTest::newRow("override:ComboBox") << "override" << "ComboBox" << (QStringList() << "combobox-override" << "combobox-background-override" << "combobox-contentItem-simple"  << "combobox-indicator-simple" << "combobox-simple"); // overrides "simple"
    QTest::newRow("override:Dial") << "override" << "Dial" << (QStringList() << "dial-override"  << "dial-background-override" << "dial-handle-override" << "dial-incomplete"); // overrides "incomplete"
    QTest::newRow("override:Label") << "override" << "Label" << (QStringList() << "label-override" << "label-background-override" << "label-simple"); // overrides "simple"
    QTest::newRow("override:RadioButton") << "override" << "RadioButton" << (QStringList() << "radiobutton-override"  << "radiobutton-background-override" << "radiobutton-contentItem-simple" << "radiobutton-indicator-override" << "radiobutton-simple"); // overrides "simple"
    QTest::newRow("override:RangeSlider") << "override" << "RangeSlider" << (QStringList() << "rangeslider-override"  << "rangeslider-background-override" << "rangeslider-first-handle-override" << "rangeslider-second-handle-override" << "rangeslider-incomplete"); // overrides "incomplete"
    QTest::newRow("override:Slider") << "override" << "Slider" << (QStringList() << "slider-override"  << "slider-background-override" << "slider-handle-override" << "slider-incomplete"); // overrides "incomplete"
    QTest::newRow("override:TextField") << "override" << "TextField" << (QStringList() << "textfield-override" << "textfield-background-override" << "textfield-simple"); // overrides "simple"
    QTest::newRow("override:TextArea") << "override" << "TextArea" << (QStringList() << "textarea-override" << "textarea-background-override" << "textarea-simple"); // overrides "simple"
}

void tst_customization::creation()
{
    QFETCH(QString, style);
    QFETCH(QString, type);
    QFETCH(QStringList, delegates);

    QQuickStyle::setStyle(testFile("styles/" + style));

    QScopedPointer<QObject> control(createControl(type));
    QVERIFY(control);

    for (const QString &delegate : delegates)
        QVERIFY2(qt_createdQObjects()->removeOne(delegate), qPrintable(delegate + " was not created as expected"));

    QVERIFY2(qt_createdQObjects()->isEmpty(), qPrintable("unexpectedly created: " + qt_createdQObjects->join(", ")));
    QVERIFY2(qt_destroyedQObjects()->isEmpty(), qPrintable("unexpectedly destroyed: " + qt_destroyedQObjects->join(", ") + " were unexpectedly destroyed"));
}

void tst_customization::comboPopup()
{
    QQuickStyle::setStyle(testFile("styles/simple"));

    {
        // test that ComboBox::popup is created when accessed
        QQmlComponent component(engine);
        component.setData("import QtQuick.Controls 2.2; ComboBox { }", QUrl());
        QScopedPointer<QQuickItem> comboBox(qobject_cast<QQuickItem *>(component.create()));
        QVERIFY(comboBox);

        QVERIFY(!qt_createdQObjects()->contains("combobox-popup-simple"));

        QObject *popup = comboBox->property("popup").value<QObject *>();
        QVERIFY(popup);
        QVERIFY(qt_createdQObjects()->contains("combobox-popup-simple"));
    }

    reset();

    {
        // test that ComboBox::popup is created when it becomes visible
        QQuickWindow window;
        window.resize(300, 300);
        window.show();
        window.requestActivate();
        QVERIFY(QTest::qWaitForWindowActive(&window));

        QQmlComponent component(engine);
        component.setData("import QtQuick.Controls 2.2; ComboBox { }", QUrl());
        QScopedPointer<QQuickItem> comboBox(qobject_cast<QQuickItem *>(component.create()));
        QVERIFY(comboBox);

        comboBox->setParentItem(window.contentItem());
        QVERIFY(!qt_createdQObjects()->contains("combobox-popup-simple"));

        QTest::mouseClick(&window, Qt::LeftButton, Qt::NoModifier, QPoint(1, 1));
        QVERIFY(qt_createdQObjects()->contains("combobox-popup-simple"));
    }
}

QTEST_MAIN(tst_customization)

#include "tst_customization.moc"
