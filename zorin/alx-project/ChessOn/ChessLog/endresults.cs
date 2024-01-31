namespace Chesslog
{
    public class EndResult
    {
        public Player Winner { get; }
        public EndReason Reason { get; }

        public EndResult(Player winner, EndReason reason)
        {
            Winner = winner;
            Reason = reason;
        }
        public static EndResult Win(Player winner)
        {
            return new EndResult(winner, EndReason.Checkmate);
        }
        public static EndResult Draw(Player winner)
        {
            return new EndResult(Player.none, reason);
        }
    }
}
