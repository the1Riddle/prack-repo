using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chesslog
{
	public class PawnPromotion : Move
	{
		public override MoveType Type => MoveType.PawnPromotion;
		public override Position FromPos { get; }
		public override Position ToPos { get; }
		private readonly PieceType newType;

		public PawnPromotion(Position from, Position to, PieceType newType)
		{
			FromPos = from;
			ToPos = to;
			this.newType = newType;
		}
		private Piece CreatePromoPiece(Player color)
		{
			return newType switch
			{
				PieceType.Knight => new Knight(color),
				PieceType.bishop => new bishop(color),
				PieceType.rook => new rook(color),
					=> new queen(color)
			};
		}
		public override void Execute(Board board)
		{
			Piece piece = board[FromPos];
			board[FromPos] = null;

			Piece promoPiece = CreatePromoPiece(pawn.Color);
			board[ToPos] = promoPiece;
		}
	}
}