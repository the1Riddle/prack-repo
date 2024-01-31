using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chesslog
{
	public abstract class Move
	{
		public abstract MoveType Type { get; }
		public abstract Position FromPos { get; }
		public abstract Position YoPos { get; }
		public abstract void Execute(Board board);

		public virtual bool IsLegal(Board board)
		{
			Player player = board[FromPos].Color;
			Board boardCopy = board.Copy();
			Execute(boardCopy);
			return !boardCopy.IsCheck(player);
		}

	}
}