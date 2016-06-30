
#include <Siv3D.hpp>
#include <vector>

using namespace std;

void Main()
{
	//true -> 黒(通れない),false -> 白(とおれる)
	int w = 31, h = 21;
	int increX = Window::Size().x / w;
	int increY = Window::Size().y / h;
	Point me = { 1,1 };
	Texture tex(L"北条丸.png", { 0,0,0 });
	const Sound sound(L"09 ここまで！ (A).mp3");
	const Sound bgm(L"yosaku.mp3");
	bool takada = true;

	Font endt(30);

	vector<vector<int>> maze(w, vector<int>(h, 0));
	for (int x = 0;x < w;x++)
	{
		maze[x][0] = 1;
		maze[x][h - 1] = 1;
	}
	for (int y = 0;y < h;y++)
	{
		maze[0][y] = 1;
		maze[w - 1][y] = 1;
	}

	for (int x = 2;x <= w - 3;x += 2)
	{
		for (int y = 2;y <= h - 3;y += 2)
		{
			maze[x][y] = 2; //ボンバーマンのように壁にする

			vector<Point> points;

			if (!maze[x - 1][y])points.push_back({ x - 1,y });
			if (!maze[x][y + 1])points.push_back({ x,y + 1 });
			if (!maze[x + 1][y])points.push_back({ x + 1,y });
			//上以外参方向を候補に追加する
			if (y == 2)points.push_back({ x,y - 1 }); //最初だけは上方向も候補にいれる

			Point p = points[Random(0, static_cast<int>(points.size() - 1))]; //ランダムで候補から一ついれる

			maze[p.x][p.y] = 3;
		}
	}
	//ここまで初期化
	bgm.play();

	while (System::Update())
	{
		for (int x = 0;x < w;x++)
		{
			for (int y = 0;y < h;y++)
			{
				Rect rect(x * increX, y * increY, increX, increY);

				if (x == 1 && y == 1) rect(tex).draw(); //スタートに、北条
				else if (x == w - 2 && y == h - 2) rect(tex).draw(); //ゴールに藻、北条
				else
				{
					switch (maze[x][y])
					{
					case 0:
						rect.draw({ 255,255,255 });
						break;
					case 1:
						rect.draw({ 0,0,0 });
						break;
					case 2:
						rect.draw({ 127,127,127 });
						break;
					case 3:
						rect.draw({ 0,0,255 });
						break;
					default:
						break;
					}
				}
				/*
				else if (maze[x][y])
				{
				rect.draw({ 0,0,0 });
				}
				else
				{
				rect.draw({ 255,255,255 });
				}
				*/
			}
		}

		if (Input::KeyRight.clicked && !maze[me.x + 1][me.y])me.x += 1;
		if (Input::KeyLeft.clicked && !maze[me.x - 1][me.y])me.x -= 1;
		if (Input::KeyUp.clicked && !maze[me.x][me.y - 1])me.y -= 1;
		if (Input::KeyDown.clicked && !maze[me.x][me.y + 1])me.y += 1;

		const int corrX = increX / 2;
		const int corrY = increY / 2;

		Circle({ (me.x * increX) + corrX,(me.y * increY) + corrY }, corrX).draw({ 255,0,0 }); //自分を描写

		//終了処理
		if ((me.x == w - 2 && me.y == h - 2 && takada))
		{
			const int32 pos = static_cast<int32>(bgm.streamPosSec());
			if (takada) takada = false;
			sound.play();
			bgm.stop();
			Rect({ 0,0 }, { 640,480 }).draw({ 0,0,0,127 });
			endt(L"END").drawCenter({ 280,240 }, { 255,0,0 });
			endt(pos, L"秒").drawCenter({ 320,300 }, { 255,0,0 });
			WaitKey();
			System::Exit();
		}
		else if (!bgm.isPlaying())
		{
			Rect({ 0,0 }, { 640,480 }).draw({ 0,0,0,127 });
			endt(L"---GAME OVER---").drawCenter({ 280,240 }, { 255,0,0 });
			WaitKey();
			System::Exit();
		}

	}
}
