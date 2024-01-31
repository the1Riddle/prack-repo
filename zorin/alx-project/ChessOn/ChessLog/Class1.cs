namespace Chesslog
{
    public class DoublePawn : Move
    {
        public override MoveType Type => MoveType.DoublePawn;
        public override Position FromPos { get; }
        public override Position ToPos { get; }

        private readonly Position skippedPos;

        public DoublePawn(Position from, Position to)
        {
            FromPos = from;
            ToPos = to;
            skippedPos = new Position((from.Row + to.Row) / 2, from.Column);
        }
        public override void Execute(Board board)
        {
            Player player = board[FromPos].Color;
            board.SetPawnSkipPos(player, skippedPos);
            new NormalMove(FromPos, ToPos).Execute(board);
        }
    }
}
