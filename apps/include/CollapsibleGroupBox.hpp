#pragma once

#include <QLayout>
#include <QPointer>
#include <QWidget>

class CollapsibleGroupBox : public QWidget
{
    Q_OBJECT
public:
    explicit CollapsibleGroupBox(QWidget* parent = nullptr);

    void setContentLayout(QLayout* layout);
    QLayout* contentLayout();

protected:
    QPointer<QWidget> content_;
};
