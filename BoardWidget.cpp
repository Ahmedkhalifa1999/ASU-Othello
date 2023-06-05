#include "BoardWidget.h"

BoardWidget::BoardWidget(Board &board, QWidget *parent)
 : QWidget{parent}, board{board}
{

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

}
