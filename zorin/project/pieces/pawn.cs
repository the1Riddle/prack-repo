using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chesslog
{
	public class Pawn : Piece
	{
		public override PieceType Type => PieceType.Pawn;
		public override Player Color { get;}

		private readonly Direction forward;

		public Pawn(Player color)
		{
			Color = color;
			if (color == Player.white)
			{
				forward = Direction.north;
			}
			else if (color == Player.black)
			{
				forward = Direction.south;
			}
		}
		public Pawn(Player color)
		{
			Color = color;
		}
		public override Piece Copy()
		{
			Pawn copy = new Pawn(Color);
			copy.HasMoved = HasMoved;
			return copy;
		}
		public override IEnumerable<Move> GetMoves(Position from, Board board)
		{
			return MovePosInDirs(from, board, dirs).Select(to => new NormalMove(from, to));
		}

		private static bool CanMoveTo(Position pos, Board board)
		{
			return Board.IsInside(pos) && board.IsEmpty(pos);
		}
		private bool CanCaptureAt(Position pos, Board board)
		{
			if (!Board.IsInside(pos) || board.IsEmpty(pos))
			{
				return false;
			}
			return Board[pos].Color != Color;
		}
		private static IEnumerable<Move> PromotionMoves(Position from, Position to)
		{
			yield return new PawnPromotion(from, to, PieceType.Knight);
			yield return new PawnPromotion(from, to, PieceType.Bishop);
			yield return new PawnPromotion(from, to, PieceType.Rook);
			yield return new PawnPromotion(from, to, PieceType.Queen);
		}
		private IEnumerable<Move> ForwardMoves(Position from, Board board)
		{
			Position MoveOnce = from + forward;

			if (CanMoveTo(MoveOnce, board))
			{
				if  (MoveOnce.Row == 0 || MoveOnce.Row == 7)
				{
					foreach (Move promMove in PromotionMoves(from, MoveOnce))
					{
						yield return promMove;
					}
				}
				else
				{
					yield return new NormalMove(from, MoveOnce);
				}

				Position MoveTwice = MoveOnce + forward;

				if (!HasMoved && CanMoveTo(MoveTwice, board))
				{
					yield return new DoublePawn(from, MoveTwice);
				}
			}
		}
		private IEnumerable<Move> DiagonalMoves(Position from, Board board)
		{
			foreach (Direction dir in new Direction[] { Direction.west, Direction.east })
			{
				Position to = from + forward + dir;

				if (to == board.GetPawnSkipPos(Color.Opponent()))
				{
					yield return new EnPassant(from, to);
				}
				else if (CanCaptureAt(to, board))
				{
					if  (to.Row == 0 || to.Row == 7)
					{
						foreach (Move promMove in PromotionMoves(from, to))
						{
							yield return promMove;
						}
					}
					else
					{
						yield return new NormalMove(from, to);
					}
				}
			}
		}
		public override IEnumerable<Move> GetMoves(Position from, Board board)
		{
			return ForwardMoves(from, board).Concat(DiagonalMoves(from, board));
		}
		public override bool CanCaptureOppKing(Position from, Board board)
		{
			return DiagonalMoves(from, board).Any(move =>
			{
				Piece piece = board[move.ToPos];
				return piece != null && piece.Type == PieceType.King;
			});
		}
	}
}