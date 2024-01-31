using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chesslog
{
	public abstract class Piece
	{
		public abstract PieceType Type { get; }
		public abstract Player Color { get; }
		public bool HasMoved { get; set; } = false;
		public abstract Piece Copy();
		public abstract IEnumerable<Move> GetMoves(Position from, Board board);

		protected IEnumerable<Position> MovePosInDir(Position from, Board board, Direction dir)
		{
			for (Position pos = from + dir; Board.IsInside(pos); pos += dir)
			{
				if (board.IsEmpty(pos))
				{
					yield return pos;
					continue;
				}
				Piece piece = board[pos];
				if (piece.Color != Color)
				{
					yield return pos;
				}
				yield break;
			}
		}
		protected IEnumerable<Position> MovePosInDir(Position from, Board board, Direction[] dirs)
		{
			return dirs.SelectMany(dir => MovePosInDir(from, board, dir));
		}
		public virtual bool CanCaptureOppKing(Position from, Board board)
		{
			return GetMoves(from, board).Any(move =>
			{
				Piece piece = board[move.ToPos];
				return piece != null && piece.Type == PieceType.King;
			});
		}
	}
}