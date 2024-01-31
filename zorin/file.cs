using System;
using Gtk;

namespace ChessBoard
{
	public class Program1 : window
	{
		public Program1() : base("Simple App")
		{
			DeleteEvent += delegate
			{
				Application.Quit();
			};
			this.Show();
		}
		public static void Main(string[] args)
		{
			Aplication.Init();
			new Program1();
			Aplication.Run();
		}
	}
}
