#pragma once

#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QPointer>
#include <QWidget>

#include "ClickableLabel.hpp"

class CollapsibleGroupBox : public QWidget
{
    Q_OBJECT
public:
    explicit CollapsibleGroupBox(QWidget* parent = nullptr);

    void setContentLayout(QLayout* layout);
    QLayout* contentLayout();

    void setTitle(const QString& t);
    QString title() const;

    bool isExpanded() const;
    void setExpanded(bool b);

public slots:
    void expand();
    void collapse();
    void toggle();

signals:
    void expanded();
    void collapsed();
    void toggled();

protected:
    QPointer<ClickableLabel> expander_;
    QPointer<QLabel> title_;
    QPointer<QWidget> content_;
    QSize contentSize_;
};
