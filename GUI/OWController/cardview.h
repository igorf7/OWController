#ifndef CARDVIEW_H
#define CARDVIEW_H

#include <QWidget>

namespace Ui {
class CardView;
}

class CardView : public QWidget
{
    Q_OBJECT

public:
    explicit CardView(QWidget *parent = nullptr);
    ~CardView();

protected:
    void paintEvent(QPaintEvent*);

private:
    Ui::CardView *ui;

    QColor color;
};

#endif // CARDVIEW_H
