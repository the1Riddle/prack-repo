namespace Chesslog
{
    public enum Player
    {
        none,
        white,
        black
    }
    public static class PlayerExts
    {
        public static Player Opponent(this Player player)
        {
            return player switch
            {
                Player.white => Player.black,
                Player.black => Player.white,
                    => Player.none,
            };
        }
    }
}
