#include "CollapsibleGroupBox.hpp"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

QString RIGHT{"►"};
QString DOWN{"▼"};

CollapsibleGroupBox::CollapsibleGroupBox(QWidget* parent) : QWidget(parent)
{
    setLayout(new QVBoxLayout());
    layout()->setMargin(0);

    auto header = new QWidget(this);
    auto hlayout = new QHBoxLayout();
    header->setLayout(hlayout);
    hlayout->setMargin(0);

    auto expander = new QLabel(RIGHT);
    QFont expanderFont;
    expanderFont.setPointSize(expanderFont.pointSize() - 4);
    expander->setFont(expanderFont);
    hlayout->addWidget(expander);

    auto details = new QLabel("Details");
    QFont font;
    font.setPointSize(font.pointSize() - 2);
    details->setFont(font);
    hlayout->addWidget(details);
    hlayout->addStretch(1);

    layout()->addWidget(header);

    content_ = new QWidget();
    layout()->addWidget(content_);
    content_->setHidden(true);
}

QLayout* CollapsibleGroupBox::contentLayout() { return content_->layout(); }

void CollapsibleGroupBox::setContentLayout(QLayout* layout)
{
    content_->setLayout(layout);
}