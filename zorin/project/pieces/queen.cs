using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chesslog
{
	public class Queen : Piece
	{
		public override PieceType Type => PieceType.Queen;
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
		public Queen(Player color)
		{
			Color = color;
		}
		public override Piece Copy()
		{
			Queen copy = new Queen(Color);
			copy.HasMoved = HasMoved;
			return copy;
		}
		public override IEnumerable<Move> GetMoves(Position from, Board board)
		{
			return MovePosInDirs(from, board, dirs).Select(to => new NormalMove(from, to));
		}
	}
}