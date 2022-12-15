#pragma once

// From https://wiki.qt.io/Clickable_QLabel

#include <QLabel>
#include <QWidget>
#include <Qt>

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(
        QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);
};
