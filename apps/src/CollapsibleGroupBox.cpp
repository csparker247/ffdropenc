#include "CollapsibleGroupBox.hpp"

#include <QDebug>
#include <QFont>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QStyle>
#include <QVBoxLayout>

QString RIGHT{"►"};
QString DOWN{"▼"};

CollapsibleGroupBox::CollapsibleGroupBox(QWidget* parent) : QWidget(parent)
{
    setLayout(new QVBoxLayout());
    layout()->setMargin(0);
    layout()->setAlignment(Qt::AlignTop);

    auto header = new QWidget(this);
    auto hlayout = new QHBoxLayout();
    header->setLayout(hlayout);
    hlayout->setMargin(0);

    expander_ = new ClickableLabel();
    expander_->setText(RIGHT);
    QFont expanderFont;
    expanderFont.setPointSize(expanderFont.pointSize() - 4);
    expander_->setFont(expanderFont);
    hlayout->addWidget(expander_);
    connect(
        expander_, &ClickableLabel::clicked, this,
        &CollapsibleGroupBox::toggle);

    title_ = new QLabel();
    QFont font;
    font.setPointSize(font.pointSize() - 2);
    title_->setFont(font);
    hlayout->addWidget(title_);
    hlayout->addStretch(1);

    layout()->addWidget(header);

    content_ = new QWidget();
    content_->setHidden(true);
    layout()->addWidget(content_);

    connect(this, &CollapsibleGroupBox::expanded, [this]() { emit toggled(); });
    connect(
        this, &CollapsibleGroupBox::collapsed, [this]() { emit toggled(); });
}

QLayout* CollapsibleGroupBox::contentLayout() { return content_->layout(); }

void CollapsibleGroupBox::setContentLayout(QLayout* layout)
{
    content_->setLayout(layout);
}

void CollapsibleGroupBox::setTitle(const QString& t) { title_->setText(t); }

QString CollapsibleGroupBox::title() const { return title_->text(); }

void CollapsibleGroupBox::expand()
{
    expander_->setText(DOWN);
    content_->setVisible(true);
    adjustSize();
    emit expanded();
}

void CollapsibleGroupBox::collapse()
{
    expander_->setText(RIGHT);
    content_->setVisible(false);
    adjustSize();
    emit collapsed();
}

void CollapsibleGroupBox::toggle()
{
    if (content_->isHidden()) {
        expand();
    } else {
        collapse();
    }
}