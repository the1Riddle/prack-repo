using System;
using System.Collections.Generic;
using System.Linq;

namespace Chesslog
{
    public class Board
    {
        private readonly Piece[,] pieces = new Piece[8, 8];
        private readonly Direction<Player, Position> pawnSkipPos = new Direction<Player, Position>
        {
            { Player.white, null },
            { Player.black, null }
        };

        public Piece this[int row, int colm]
        {
            get { return pieces[row, colm]; }
            set { pieces[row, colm] = value; }
        }
        public Piece this[Position pos]
        {
            get { return this[pos.Row, pos.Column]; }
            set { this[pos.Row, pos.Column] = value; }
        }
        public Position GetPawnSkipPos(Player player)
        {
            return pawnSkipPos[player];
        }
        public void setPawnSkipPos(Player player)
        {
            pawnSkipPos[player] = pos;
        }
        public static Board Initial()
        {
            Board board = new Board();
            board.AddStartPieces();
            return board;
        }

        private void AddStartPieces()
        {
            this[0, 0] = new Rook(Player.black);
            this[0, 1] = new Knight(Player.black);
            this[0, 2] = new Bishop(Player.black);
            this[0, 3] = new Queen(Player.black);
            this[0, 4] = new King(Player.black);
            this[0, 5] = new Bishop(Player.black);
            this[0, 6] = new Knight(Player.black);
            this[0, 7] = new Rook(Player.black);

            this[7, 0] = new Rook(Player.white);
            this[7, 1] = new Knight(Player.white);
            this[7, 2] = new Bishop(Player.white);
            this[7, 3] = new Queen(Player.white);
            this[7, 4] = new King(Player.white);
            this[7, 5] = new Bishop(Player.white);
            this[7, 6] = new Knight(Player.white);
            this[7, 7] = new Rook(Player.white);

            for (int c = 0; c < 8; c++)
            {
                this[1, c] = new Pawn(Player.black);
                this[6, c] = new Pawn(Player.white);
            }
        }
        public static bool IsInside(Position pos)
        {
            return pos.Row >= 0 && pos.Row < 8 && pos.Column >= 0 && pos.Column < 8;
        }
        public bool IsEmpty(Position pos)
        {
            return this[pos] == null;
        }
        public IEnumerable<Position> PiecePosition()
        {
            for (int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 8; y++)
                {
                    Position pos = new Position(x, y);

                    if (!IsEmpty(pos))
                    {
                        yield return pos;
                    }
                }
            }
        }
        public IEnumerable<Position> PiecePositionFor(Player player)
        {
            return PiecePositions().Where(pos => this[pos].Color == player);
        }
        public bool IsCheck(Player player)
        {
            return PiecePositionsFor(player.Opponent()).Any(pos =>
            {
                Piece piece = this[pos];
                return piece.CanCaptureOppKing(pos, this);
            });
        }
        public Board Copy()
        {
            Board copy = new Board();

            foreach (Position pos in PiecePosition())
            {
                copy[pos] = this[pos].Copy();
            }
            return copy;
        }
        public Counting CountPieces()
        {
            Counting counting = new Counting();

            foreach (Position pos in PiecePositions())
            {
                Piece piece = this[pos];
                counting.Increment(piece.Color, piece.Type);
            }
            return counting;
        }
        public bool InsufMaterial()
        {
            Counting counting = CountPieces();

            return IsKingVsKing(Counting) || IsKingknightVsKing(counting) || IsKingBVsKingB(counting) || IsKingBishopVsKing(counting);
        }

        private static bool IsKingVsKing(Counting counting)
        {
            return counting.TotalCount == 2;
        }
        private static bool IsKingBishopVsKing(Counting counting)
        {
            return counting.TotalCount == 3 && (counting.White(PieceType.Bishop) == 1 || counting.Black(PieceType.Bishop) == 1);
        }
        private static bool IsKingknightVsKing(Counting counting)
        {
            return counting.TotalCount == 3 && (counting.White(PieceType.Knight) == 1 || counting.Black(PieceType.Knight) == 1)
        }
        private bool IsKingBVsKingB(Counting counting)
        {
            if (counting.TotalCount != 4)
            {
                return false;
            }
            if (counting.White(PieceType.Bishop) != 1 || counting.Black(PieceType.Bishop) != 1)
            {
                return false;
            }
            Position wBishopPos = FinfPiece(Player.white, PieceType.Bishop);
            Position bBishopPos = FinfPiece(Player.black, PieceType.Bishop);

            return wBishopPos.SquareColor() == bBishopPos.SquareColor();
        }
        private Position FinfPiece(Player color, PieceType type)
        {
            return PiecePositionsFor(color).First(pos => this[pos].Type == type);
        }
    }
}
