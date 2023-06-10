#include "BoardWidget.h"

BoardWidget::BoardWidget(Board &board, QWidget *parent)
 : QWidget{parent}, board{board}
{
    this->setFixedSize(801, 801);

    this->update();
}

void BoardWidget::paintEvent(QPaintEvent *event)
{

    std::vector<std::vector<BoardSquareState>> boardvector = board.getBoard();
    std::vector<Move> moves;
    QPainter painter(this);

    //Define board background color
    QBrush backgroundbrush = QBrush(QColor::fromRgb(7,130,81),Qt::SolidPattern);

    //Define black player brush
    QBrush blackBrush = QBrush(Qt::black,Qt::SolidPattern);

    //Define white player brush
    QBrush whiteBrush = QBrush(Qt::white,Qt::SolidPattern);

    //Define valid moves brush
    QBrush transparent = QBrush(QColor::fromRgb(0,0,0,20),Qt::SolidPattern);


    //Set pen
    QPen pen = QPen(Qt::black,0);
    painter.setPen(pen);

    //Set Board Background
    painter.setBrush(backgroundbrush);
    painter.setBackgroundMode(Qt::OpaqueMode);

    //Draw and Scale Board
    //Distance between each two points is 100
    painter.scale(100,100);
    painter.drawRect(QRect(QPoint(0,0),QPoint(7,7)));

    //Horizontal Lines
    painter.drawLine(QLine(QPoint(0,1),QPoint(8,1)));
    painter.drawLine(QLine(QPoint(0,2),QPoint(8,2)));
    painter.drawLine(QLine(QPoint(0,3),QPoint(8,3)));
    painter.drawLine(QLine(QPoint(0,4),QPoint(8,4)));
    painter.drawLine(QLine(QPoint(0,5),QPoint(8,5)));
    painter.drawLine(QLine(QPoint(0,6),QPoint(8,6)));
    painter.drawLine(QLine(QPoint(0,7),QPoint(8,7)));
    //Vertical Lines
    painter.drawLine(QLine(QPoint(1,0),QPoint(1,8)));
    painter.drawLine(QLine(QPoint(2,0),QPoint(2,8)));
    painter.drawLine(QLine(QPoint(3,0),QPoint(3,8)));
    painter.drawLine(QLine(QPoint(4,0),QPoint(4,8)));
    painter.drawLine(QLine(QPoint(5,0),QPoint(5,8)));
    painter.drawLine(QLine(QPoint(6,0),QPoint(6,8)));
    painter.drawLine(QLine(QPoint(7,0),QPoint(7,8)));




    //Print Board
    for(int row=0;row<8;row++)
    {
        for(int col=0 ; col < 8; col++)
        {
            if(boardvector[row][col] == WHITE_DISK)
            {
                painter.setBrush(whiteBrush);
                painter.drawEllipse(QRectF(QPointF(row,col),QPointF(row+1,col+1)));
            }
            else if(boardvector[row][col] == BLACK_DISK)
            {
                painter.setBrush(blackBrush);
                painter.drawEllipse(QRectF(QPointF(row,col),QPointF(row+1,col+1)));
            }
        }

    }


    //Print Possible Valid Moves
    moves = board.getValidMoves(BLACK_PLAYER);
    for(auto move : moves)
    {
        painter.setBrush(transparent);
        painter.drawEllipse(QRectF(QPointF(move.row,move.column),QPointF(move.row+1,move.column+1)));

    }




}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
    unsigned int row , column;

    Move move;

    QPoint p; 

    // 1. Get the position (x,y) of the move, where the mouse left button is pressed

      p = event->pos();
    
    // Get Column

    if((p.x() >=0) && (p.x()<=100) ){
        column=0;
    }
    else if((p.x() >=101) && (p.x()<=200)){
        column=1;
    }
    else if((p.x() >=201) && (p.x()<=300)){
        column=2;
    }
    else if((p.x() >=301) && (p.x()<=400)){
        column=3;
    }
    else if((p.x() >=401) && (p.x()<=500)){
        column=4;
    }
    else if((p.x() >=501) && (p.x()<=600)){
        column=5;
    }
    else if((p.x() >=601) && (p.x()<=700)){
        column=6;
    }
    else if((p.x() >=701) && (p.x()<=801)){
        column=7;
    }

    // Get Row

    if((p.y() >=0) && (p.y()<=100) ){
        row=0;
    }
    else if((p.y() >=101) && (p.y()<=200)){
        row=1;
    }
    else if((p.y() >=201) && (p.y()<=300)){
        row=2;
    }
    else if((p.y() >=301) && (p.y()<=400)){
        row=3;
    }
    else if((p.y() >=401) && (p.y()<=500)){
        row=4;
    }
    else if((p.y() >=501) && (p.y()<=600)){
        row=5;
    }
    else if((p.y() >=601) && (p.y()<=700)){
        row=6;
    }
    else if((p.y() >=701) && (p.y()<=801)){
        row=7;
    }

    move.row = row;

    move.column = column;

    // 2. Get the color of the player who wanted to do the move



    // 3. do the desired moved
 
    //   doMove( move, player);


}
