namespace Chesslog
{
	public class Counting
	{
		private readonly Dictionary<PieceType, int> whiteCount = new();
		private readonly Direction<PieceType, int> blackCount = new();

		public Counting()
		{
			foreach (PieceType type in Enum.GetValues(typeof(PieceType)))
			{
				whiteCount[type] = 0;
				blackCount[type] = 0;
			}
		}
		public void Increment(Player color, PieceType type)
		{
			if (color == Player.white)
			{
				whiteCount[type]++;
			}
			else if (color == Player.black)
			{
				blackCount[type]++;
			}
			TotalCount++;
		}
		public int White(PieceType type)
		{
			return whiteCount[type];
		}
		public int Black(PieceType type)
		{
			return blackCount[type];
		}
	}
}