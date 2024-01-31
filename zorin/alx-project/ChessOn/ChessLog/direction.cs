namespace Chesslog
{
    public class Direction
    {
        public readonly static Direction north = new Direction(-1, 0);
        public readonly static Direction south = new Direction(1, 0);
        public readonly static Direction east = new Direction(0, 1);
        public readonly static Direction west = new Direction(0, -1);
        public readonly static Direction northEast = north + east;
        public readonly static Direction northWest = north + west;
        public readonly static Direction southEast = south + east;
        public readonly static Direction southWest = south + west;

        public int RowDelta { get; }
        public int ColumnDelta { get; }

        public Direction(int rowDelta, int columnDelta)
        {
            RowDelta = rowDelta;
            ColumnDelta = columnDelta;
        }
        public static Direction operator +(Direction dir1, Direction dir2)
        {
            return new Direction(dir1.RowDelta + dir2.RowDelta, dir1.ColumnDelta + dir2.ColumnDelta);
        }
        public static Direction operator *(int scalar, Direction dir)
        {
            return new Direction(scalar * dir.RowDelta, scalar * dir.ColumnDelta);
        }
    }
}