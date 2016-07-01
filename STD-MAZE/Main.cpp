
#include <Siv3D.hpp>
#include <vector>

using namespace std;

void Main()
{
	//true -> 黒(通れない),false -> 白(とおれる)
	const int w = 31, h = 21;
	const int increX = Window::Size().x / w;
	const int increY = Window::Size().y / h;
	const int corrX = increX / 2;
	const int corrY = increY / 2;

	Point me = { 1,1 };
	Point xroad = me;

	const Texture hozyo(L"hozyo.png", { 0,0,0 });
	const Sound sound(L"TakadaWait.mp3");
	const Sound bgm(L"yosaku.mp3");
	bool takada = true;

	Font endt(30);
	Font goalF(increY - (increY / 5));

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

			//上以外参方向を候補に追加する
			if (maze[x - 1][y] == 0)points.push_back({ x - 1,y });
			if (maze[x][y + 1] == 0)points.push_back({ x,y + 1 });
			if (maze[x + 1][y] == 0)points.push_back({ x + 1,y });

			if (y == 2)points.push_back({ x,y - 1 }); //最初だけは上方向も候補にいれる

			Point p = points[Random(0, static_cast<int>(points.size() - 1))]; //ランダムで候補から一ついれる

			maze[p.x][p.y] = 3;
		}
	}
	//ここまで初期化
	bgm.play();

	while (System::Update())
	{
		xroad = me;

		//プレイヤー移動
		if (Input::KeyRight.clicked && maze[me.x + 1][me.y] == 0)me.x += 1;
		if (Input::KeyLeft.clicked && maze[me.x - 1][me.y] == 0)me.x -= 1;
		if (Input::KeyUp.clicked && maze[me.x][me.y - 1] == 0)me.y -= 1;
		if (Input::KeyDown.clicked && maze[me.x][me.y + 1] == 0)me.y += 1;

		if (xroad != me) maze[xroad.x][xroad.y] = 4; //すでに進んだ場所を通行不可に

		for (int x = 0;x < w;x++)
		{
			for (int y = 0;y < h;y++)
			{
				Rect rect(x * increX, y * increY, increX, increY);

				if (x == w - 2 && y == h - 2)
					goalF(L"G").drawCenter({ x * increX + corrX,y * increY  + corrY}, { 255,255,0 }); //ゴールにGマーク
				else
				{
					switch (maze[x][y])
					{
					case 0:
						rect.draw({ 255,255,255 }); //進める場所は白
						break;
					case 1:
						rect.draw({ 0,0,0 }); //壁は黒
						break;
					case 2:
						rect.draw({ 127,127,127 }); //最初の棒は灰色
						break;
					case 3:
						rect.draw({ 0,0,255 }); //倒した棒は青色
						break;
					case 4:
						rect(hozyo).draw(); //すでに進んだ場所は北条
						break;
					default:
						break;
					}
				}
			}
		}

		Circle({ (me.x * increX) + corrX,(me.y * increY) + corrY }, corrX).draw({ 255,0,0 }); //自分を描写

		const bool over = (maze[me.x][me.y + 1] != 0 && maze[me.x][me.y - 1] != 0 
						&& maze[me.x + 1][me.y] != 0 && maze[me.x - 1][me.y] != 0); //上下左右いけない

		//終了処理
		if ((me.x == w - 2 && me.y == h - 2 && takada)) //ゴールしたら[くりあ]
		{
			const int32 pos = static_cast<int32>(bgm.streamPosSec());
			if (takada) takada = false;
			sound.play();
			bgm.stop();
			Rect({ 0,0 }, { 640,480 }).draw({ 0,0,0,127 });
			endt(L"---STAGE CLEAR---").drawCenter({ 280,240 }, { 255,0,0 });
			endt(pos, L"秒").drawCenter({ 320,300 }, { 255,0,0 });
			WaitKey();
			System::Exit();
		}
		else if (!bgm.isPlaying() || over) //時間切れまたはつんだら[げーむおーばー]
		{
			Rect({ 0,0 }, { 640,480 }).draw({ 0,0,0,127 });
			endt(L"---GAME OVER---").drawCenter({ 280,240 }, { 255,0,0 });
			WaitKey();
			System::Exit();
		}
	}
}
