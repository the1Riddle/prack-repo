using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chesslog
{
	public class King : Piece
	{
		public override PieceType Type => PieceType.King;
		public override Player Color { get; }

		private static readonly Direction[] dirs = new Direction[]
		{
			Direction.north,
			Direction.south,
			Direction.east,
			Direction.west,
			Direction.northEast,
			Direction.northWest,
			Direction.southEast,
			Direction.southWest
		};
		public King(Player color)
		{
			Color = color;
		}
		private static bool IsUnmovedRook(Position pos, Board board)
		{
			if (board.IsEmpty(pos))
			{
				return false;
			}
			Piece piece = board[pos];
			return piece.Type == PieceType.Rook && !piece.HasMoved;
		}
		private static bool AllEmpty(IEnumerable<Position> positions, Board board)
		{
			return positions.All(pos => board.IsEmpty(pos));
		}
		private bool CanCastleKingSide(Position from, Board board)
		{
			if (HasMoved)
			{
				return false;
			}
			Position rookPos = new Position(from.Row, 7);
			Position[] betwenPositions = new Position[] { new(from.Row, 5), new(from.Row, 6) };/* check if return syntax error */
			return IsUnmovedRook(rookPos, board) && AllEmpty(betwenPositions, board);
		}
		private bool CanCastleQueenSide(Position from, Board board)
		{
			if (HasMoved)
			{
				return false;
			}
			Position rookPos = new Position(from.Row, 0);
			Position[] betwenPositions = new Position[] { new(from.Row, 1), new(from.Row, 2), new(from.Row, 3) };/* check if return syntax error */
			return IsUnmovedRook(rookPos, board) && AllEmpty(betwenPositions, board);
		}

		public override Piece Copy()
		{
			King copy = new King(Color);
			copy.HasMoved = HasMoved;
			return copy;
		}
		private IEnumerable<Position> MovePositions(Position from, Board board)
		{
			foreach (Direction dir in dirs)
			{
				Position to = from + dir;

				if (!Board.IsInside(to))
				{
					continue;
				}
				if (board.IsEmpty(to) || board[to].Color != Color)
				{
					yield return to;
				}
			}
		}
		public override IEnumerable<move> GetMoves(Position from, Board board)
		{
			foreach (Position to in MovePositions(from, board))
			{
				yield return new NormalMove(from, to);
			}
			if (CanCastleKingSide(from, board))
			{
				yield return new Castle(MoveType.CastleKS, from);
			}
			if (CanCastleQueenSide(from, board))
			{
				yield return new Castle(MoveType.CastleQS, from);
			}
		}
		public override bool CanCaptureOppKing(Position from, Board board)
		{
			return DiagonalMoves(from, board).Any(to =>
			{
				Piece piece = board[to];
				return piece != null && piece.Type == PieceType.King;
			});
		}
	}
}