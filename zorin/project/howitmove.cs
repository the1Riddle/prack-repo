using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chesslog
{
	public enum MoveType
	{
		normal,
		castleKingS,
		castleQueenS,
		doublePawn,
		enPassant,
		pawnsPromotion,
	}
}