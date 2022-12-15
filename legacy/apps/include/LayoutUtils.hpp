#pragma once

#include <tuple>

#include <QPointer>
#include <QVBoxLayout>
#include <QWidget>

template <class Layout = QVBoxLayout, class Widget = QWidget>
std::tuple<QPointer<Widget>, QPointer<Layout>> CreateContainerWidget()
{
    QPointer<Widget> w = new Widget();
    QPointer<Layout> l = new Layout();
    w->setLayout(l);
    return {w, l};
}