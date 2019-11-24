#define _CRT_SECURE_NO_WARNINGS
#include"DxLib.h"

#define TITLE 0
#define MENU 1
#define DICE 2

#define ONEMAPNUM 90
#define TWOMAPNUM 81
#define THREEMAPNUM 97


#define STATENUM 10

#define HAPPEN 6
#define ANGEL 7//エンジェルチャンス

#define GOAL 100

#define NAKANO 60*0//0,14,26,29,71,77,80
#define HINO 60*14
#define OTSUKI 60*26
#define KAIYAMATO 60*29
#define MISATO 60*71
#define TANAKA 60*77
#define UEDA 60*80


#define IMAI 60*0//0,5,10,49,80
#define HIJIRI 60*5
#define MATUMOTO 60*10
#define HIMEKAWA 60*49

#define YOKOYAMA 60*0//0,17,35,43,69,74,81
#define KOMATU 60*17
#define NANJO 60*35
#define MAKINO 60*43
#define TAKATUKI 60*69
#define KISHIBE 60*74
#define TATIBANA 60*81

int Key[256];
int graph[100];
int music[10];
int UpdateKey() {
	char tmpKey[256];
	GetHitKeyStateAll(tmpKey);
	for (int i = 0; i<256; i++) {
		if (tmpKey[i] != 0) {
			Key[i]++;
		}
		else {
			Key[i] = 0;
		}
	}
	return 0;
}

//初期化
void init();
//状況
byte state;

//色
int white = GetColor(255, 255, 255);
int GreenA = GetColor(109, 247, 156);
int GreenB = GetColor(0, 255, 65);
int Glay = GetColor(140, 140, 140);
int red = GetColor(240, 25, 25);
int blue = GetColor(67, 125, 230);
int yellow = GetColor(251, 239, 35);
int skyblue = GetColor(48, 0, 252);
int orange = GetColor(255, 129, 25);
int purple = GetColor(142, 0, 204);
void boxcolor();

//GOALチェック
void goalcheck();

//ボタン移動
void screenmove();

//タイトル画面
void title();

//メニュー画面
void menu();
struct MENUSTATE
{
	byte level;
}Menu;

//時間
int time;

//ダイス画面
void diceinit();
void dice();

//音楽
void initmusic();

void initmusic() {
	music[0] = LoadSoundMem("bgm8.ogg");
	
}

struct DICESTATE
{
	byte state;//場面状況

	int HP;//体力値
	
	int HPdrink;//体力回復ドリンク所持数
	int money;//所持金
	int rapidcard;//快速カード所持数
	int expresscard;//急行カード所持数

	byte happening;//ハプニング有無
	byte gonum;//進行数
	int i;//繰り返し用関数
	int charnum;//文字番号
	int charY;//駅名スクロールY
	int PlayerLocationY;//プレイヤーY位置
	int dice;//サイコロの目
	byte extend;//サイコロの倍率
	int dicedecision;//サイコロの目確定
	int dicedecisionA;//nマス戻る用のサイコロの目確定
	
	int getmoney;//獲得マニー
	int mapstate;//マップ状況
	int turn;//ターン
	byte happenstate;//ハプニングタイム状況
	byte angelstate;//エンジェルタイム状況
	bool nextone;//強制出目1フラグ
	byte speed;//ゲームのスピード

}Dice;

void map();//駅名表示

void status();//ステイタス表示
void happening();//ハプニングタイム
void angel();//エンジェルタイム
int pointlim(int mapstate) {//ステージごとの駅数の値
	if (mapstate == 0)
	{
		return ONEMAPNUM;
	}
	else if(mapstate == 1)
	{
		return TWOMAPNUM;
	}
	else
	{
		return THREEMAPNUM;
	}
	
}
int limited(int charnum) {
	return 7;
}
void init() {
	initmusic();
	StopMusic();
	state = TITLE;
	Dice.state = 0;
	Dice.turn = 0;
	Dice.charY = 0;
	Dice.nextone = false;
	Dice.speed = 1;
}

//ファイル管理
FILE *fp;//ファイル
char filename[10] = "data.bin";

//セーブデータ内に記録するための変数
struct FILEDATA {
	int amacount = 0;//甘口クリア回数
	int tyucount = 0;//中辛クリア回数
	int karacount = 0;//辛口クリア回数
	int charnum;//文字番号
	int charY;//駅名スクロールY
	int PlayerLocationY;//プレイヤーY位置
	byte extend;//サイコロの倍率
	int turn;//ターン
	int mapstate;//何ステージ目か
	int HP; 
	int HPdrink;
	int money;
	int rapidcard;
	int expresscard;
	int speed;//ゲームスピード
	bool nextone;
	byte savedata = 0;//セーブデータファイルあるか(0:確認前 1:ある 2:ない)
	bool saveOK = false;//ゲーム中にセーブしたかフラグ
	bool togoal;//ゴールした直後か
}fdata;


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	SetOutApplicationLogValidFlag(false);//デバック情報用ログファイル(Log.txt)出力しないように
	SetBackgroundColor(190, 100, 255);
	ChangeWindowMode(TRUE), SetMainWindowText("ウルトラトラベル!.exe"), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK);

	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);
	init();
	ChangeFont("メイリオ");
	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && UpdateKey() == 0) {
		screenmove();
		switch (state)
		{
		case TITLE:
			title();
			break;
		case MENU:
			menu();
			break;
		case DICE:
			dice();
			break;
		}
		//DrawFormatString(0, 0, GreenA, "%d,%d,%d,%d", fdata.HP,fdata.charnum,fdata.charY,fdata.money);
	}

	DxLib_End();
	return 0;
}
char levelstr[3][5]{
	"甘口","中辛","辛口"
};

char StationOfTohokuline[ONEMAPNUM+8][16]{
	"中野","高円寺","阿佐ヶ谷","荻窪","西荻窪","吉祥寺","三鷹","武蔵境","東小金井","武蔵小金井",
	"国分寺","西国分寺","国立","立川","日野","豊田","八王子","西八王子","高尾","相模湖",
	"藤野","上野原","四方津","梁川","鳥沢","猿橋","大月","初狩","笹子","甲斐大和",
	"勝沼ぶどう郷","塩山","東山梨","山梨市","春日居町","石和温泉","酒折","甲府","竜王","塩崎",
	"韮崎","新府","穴山","日野春","長坂","小淵沢","甲斐小泉","甲斐大泉","清里","野辺山",
	"信濃川上","佐久広瀬","佐久海ノ口","海尻","松原湖","小海","馬流","高岩","八千穂","海瀬",
	"羽黒下","青沼","臼田","龍岡城","太田部","中込","滑津","北中込","岩村田","佐久平",
	"中佐都","美里","三岡","乙女","東小諸","小諸","滋野","田中","大屋","信濃国分寺",
	"上田","西上田","テクノさかき","坂城","戸倉","千曲","屋代","屋代高校前","篠ノ井","今井",NULL
};//0,14,26,29,71,77,80

char StationOfTokaidoline[TWOMAPNUM+8][16]{
	"今井","篠ノ井","稲荷山","姨捨","冠着","聖高原","坂北","西条","明科","田沢",
	"松本","北松本","島内","島高松","梓橋","一日市場","中萱","南豊科","豊科","柏矢町",
	"穂高","有明","安曇追分","細野","北細野","信濃松川","安曇沓掛","信濃常盤","南大町","信濃大町",
	"北大町","信濃木崎","稲尾","海ノ口","簗場","南神城","神城","飯森","白馬","信濃森上",
	"白馬大池","千国","南小谷","中土","北小谷","平岩","小滝","根知","頸城大野","姫川",
	"糸魚川","青海","親不知","市振","越中宮崎","泊","入善","西入善","生地","黒部",
	"魚津","東滑川","滑川","水橋","東富山","富山","呉羽","小杉","越中大門","高岡",
	"高岡やぶなみ","西高岡","福岡","石動","倶利伽羅","津幡","中津幡","本津幡","能瀬","宇野気",
	"横山",NULL
};//0,5,10,49

char StationOfMountline[THREEMAPNUM + 8][16]{
	"横山","宇野気","能瀬","本津幡","中津幡","津幡","森本","東金沢","金沢","西金沢",
	"野々市","松任","加賀笠間","美川","小舞子","能美根上","明峰","小松","粟津","動橋",
	"加賀温泉","大聖寺","牛ノ谷","細呂木","芦原温泉","丸岡","春江","森田","福井","越前花堂",
	"大土呂","北鯖江","鯖江","武生","王子保","南条","湯尾","今庄","南今庄","敦賀",
	"新疋田","近江塩津","永原","マキノ","近江中庄","近江今津","新旭","安曇川","近江高島","北小松",
	"近江舞子","比良","志賀","蓬莱","和邇","小野","堅田","おごと温泉","比叡山坂本","唐崎",
	"大津京","山科","京都","西大路","桂川","向日町","長岡京","山崎","島本","高槻",
	"摂津富田","JR総持寺","茨木","千里丘","岸辺","吹田","東淀川","新大阪","大阪","塚本",
	"尼崎","立花","甲子園口","西宮","さくら夙川","芦屋","甲南山手","摂津本山","住吉","六甲道",
	"摩耶","灘","三ノ宮","春日野道","岩屋","西灘","大石",NULL
};//0,17,35,43,69,74,81

void title() {
	SetFontSize(30);
	boxcolor();
	DrawString(120, 120, "ウルトラトラベル!", white);
	DrawString(100, 300, "メニュー:PUSH SPACE", white);
	//以下セーブデータ取得処理
	if (fdata.savedata == 0) {
		DrawString(100, 350, "セーブデータを読み込む:PUSH I", white);
	}
	else if (fdata.savedata == 1) {
		DrawString(100, 350, "セーブデータを読み込みました", white);
	}
	else if (fdata.savedata == 2 && (fdata.amacount + fdata.tyucount + fdata.karacount) == 0) {
		DrawString(100, 350, "セーブデータがありません", white);
	}
	//ここまでセーブデータ取得処理
}
void menu() {
	SetFontSize(30);
	boxcolor();
	DrawString(120, 20, "メニュー", white);
	DrawFormatString(100, 200, white, "%d周目スタート:PUSH S", fdata.amacount+ fdata.tyucount+ fdata.karacount+ 1);
	DrawString(100, 250, "タイトル:PUSH T", white);
	if (fdata.savedata == 1 && fdata.togoal == false) {
		DrawString(0, 300, "セーブポイントからスタート:PUSH N", white);
	}
	else {
		DrawString(0, 300, "セーブポイントからスタート:PUSH N", Glay);
	}
	
	
	DrawFormatString(100, 370, white, "レベル:%s(UPキーで調整)", levelstr[Menu.level]);
	if (Menu.level == 0) {
		DrawFormatString(100, 410, white, "クリア回数:%d回", fdata.amacount);
	}
	else if (Menu.level == 1) {
		DrawFormatString(100, 410, white, "クリア回数:%d回", fdata.tyucount);
	}
	else{
		DrawFormatString(100, 410, white, "クリア回数:%d回", fdata.karacount);
	}
	DrawFormatString(100, 450, white, "ゲームスピード:%d(右キーで調整)", Dice.speed);
}
void diceinit() {
	if (Dice.speed == 1) {
		Dice.speed = 14;
	}
	else if (Dice.speed == 2) {
		Dice.speed = 9;
	}
	else {
		Dice.speed = 5;
	}

	if (Menu.level == 0)//甘口
	{
		Dice.HP = 50 + GetRand(5);
		PlayMusic("bgm8.ogg", DX_PLAYTYPE_LOOP);
		Dice.HPdrink = 4;
		Dice.money = 10000;
		Dice.rapidcard = 3;
		Dice.expresscard = 1;
	}
	else if (Menu.level == 1)//中辛
	{
		Dice.HP = 40 + GetRand(5);
		PlayMusic("bgm8.ogg", DX_PLAYTYPE_LOOP);
		Dice.HPdrink = 1;
		Dice.money = 5000;
		Dice.rapidcard = 1;
		Dice.expresscard = 0;
	}
	else//辛口
	{
		Dice.HP = 35 - GetRand(5);
		PlayMusic("bgm8.ogg", DX_PLAYTYPE_LOOP);
		Dice.HPdrink = 0;
		Dice.money = 1000;
		Dice.rapidcard = 0;
		Dice.expresscard = 0;
	}
	time = 0;
	Dice.mapstate = 0;
	
}
void dice() {
	//背景色
	if (Dice.turn % 5 == 0)
	{
		SetBackgroundColor(66, 0, 122);
	}
	else if (Dice.turn % 5 == 1)
	{
		SetBackgroundColor(156, 167, 22);
	}
	else if (Dice.turn % 5 == 2)
	{
		SetBackgroundColor(250, 167, 222);
	}
	else if (Dice.turn % 5 == 3)
	{
		SetBackgroundColor(200, 74, 25);
	}
	else
	{
		SetBackgroundColor(0, 0, 57);
	}
	
	switch (Dice.state)
	{
	case 0:
		if (Dice.HP<=0)
		{
			SetFontSize(30);
			DrawString(165, 100, "力尽きてしまった・・・", red);
			SetFontSize(25);
			DrawString(200, 300, "タイトル画面:T", red);
		
		}
		else
		{
			map();
			status();
			DrawString(300, 185, "ダイスを振る:A", white);
			DrawString(300, 215, "バイトをする:B", white);
			DrawString(300, 245, "アイテムを使う:C", white);
			DrawString(300, 275, "買い物をする:D", white);
			DrawString(300, 305, "セーブする:E", white);
			if (fdata.saveOK) {
				DrawString(0, 440, "セーブしました", white);
			}
		}
		
		break;
	case 1://バイト
		
		map();
		status();
		DrawFormatString(300, 185, white, "%d円獲得した!", Dice.getmoney);
		DrawString(300, 215, "PUSH C", red);
		break;
	case 2://アイテム使用
		
		map();
		status();
		DrawString(300, 185, "HPドリンク:A", white);
		DrawString(300, 215, "快速カード:B", white);
		DrawString(300, 245, "急行カード:C", white);
		DrawString(300, 275, "戻る:M", white);
		break;
	case 3://ダイス振り
		
		map();
		status();
		if (Dice.mapstate == 0)
		{
			Dice.dice = GetRand(8) + 1;
		}
		else
		{
			Dice.dice = GetRand(7) + 1;
		}
		DrawString(300, 185, "ダイスを止める!:S", yellow);
		SetFontSize(30);
		DrawFormatString(390, 225, GreenB, "%d", Dice.dice*Dice.extend);
		
		break;
	case 4://進行
		map();
		status();
		SetFontSize(30);
		DrawFormatString(330, 215, GreenB, "%d", Dice.dicedecision);
		break;
	case GOAL:
		SetFontSize(30);
		DrawString(200, 100, "おめでとう!", red);
		DrawString(150, 200, "目的地にたどり着いた!", white);
		SetFontSize(25);
		if (Dice.mapstate == 0 || Dice.mapstate == 1)
		{
			DrawString(200, 250, "少しHPが回復した!", GreenA);
			DrawString(200, 300, "次のステージへ:T", red);
		}
		else if (Dice.mapstate == 2)
		{
			DrawString(200, 300, "タイトル画面:T", red);
		}
		
		break;
	case 5://買い物
		
		map();
		status();
		DrawString(300, 185, "HPドリンク(5000円):A", white);
		DrawString(300, 215, "快速カード(10000円):B", white);
		DrawString(300, 245, "急行カード(20000円):C", white);
		DrawString(300, 275, "戻る:M", white);
		break;
	case HAPPEN:
		happening();
		if (Dice.happenstate == 0)
		{
			SetFontSize(30);
			DrawString(160, 100, "ハプニングタイム!", red);
			if (Dice.charnum <= 50)
			{
				Dice.dice = GetRand(6 + Dice.charnum / 10);
			}
			else
			{
				Dice.dice = GetRand(14);
			}
			DrawString(110, 270, "その運命を定めよ!:Push S", yellow);
		}
		if (Dice.happenstate == 1)
		{
			time++;
			SetFontSize((time > 60) ? time - 30 : 30);//時間経過で文字が大きくなるはず
			if (time==90)
			{
				switch (Dice.dicedecision)
				{
				case 0:
					Dice.HPdrink++;
					Dice.HP--;
					break;
				case 1:
					Dice.money += 5000;
					Dice.HP--;
					break;
				case 2:
					Dice.HP -= 3;
					break;
				case 3:
					Dice.HP += 2;
					break;
				case 4:
					Dice.money += 1000;
					Dice.HP--;
					break;
				case 5:
					Dice.HP /= 2;
					break;
				case 6:
					Dice.nextone = true;
					Dice.HP--;
					break;
				case 7:
					Dice.money += 10000;
					Dice.HP--;
					break;
				case 8:
					Dice.money *= 2;
					Dice.HP--;
					break;
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
					Dice.HP--;
					Dice.charY += 60 * Dice.dicedecisionA;
					Dice.PlayerLocationY -= 60 * Dice.dicedecisionA;
					if (Dice.PlayerLocationY <= 15)
					{
						Dice.charY = 0;
						Dice.PlayerLocationY = 15;
					}
					break;
				}
				Dice.turn++;
				Dice.state = 0;
				Dice.happenstate = 0;
				time = 0;
			}
			
		}
		break;
	case ANGEL:
		angel();
		if (Dice.angelstate == 0)
		{
			SetFontSize(30);
			DrawString(200, 100, "導かれし者!", GreenB);
			if (Dice.mapstate == 0)
			{
				Dice.dice = GetRand(7);
			}
			else if (Dice.mapstate == 1)
			{
				Dice.dice = GetRand(6);
			}
			else if (Dice.mapstate == 2)
			{
				Dice.dice = GetRand(6);
			}
			
			DrawString(110, 270, "導かれし道を定めよ!:Push S", yellow);
		}
		if (Dice.angelstate == 1)
		{
			time++;
			SetFontSize((time > 60) ? time - 30 : 30);//時間経過で文字が大きくなるはず
			if (time==90)
			{
				if (Dice.mapstate == 0)
				{
					switch (Dice.dicedecision)
					{
					case 0:
						Dice.charY = -NAKANO;
						Dice.PlayerLocationY = 15 + NAKANO;
						break;
					case 1:
						Dice.charY = -HINO;
						Dice.PlayerLocationY = 15 + HINO;
						break;
					case 2:
						Dice.charY = -OTSUKI;
						Dice.PlayerLocationY = 15 + OTSUKI;
						break;
					case 3:
						Dice.charY = -KAIYAMATO;
						Dice.PlayerLocationY = 15 + KAIYAMATO;
						break;
					case 4:
						Dice.charY = -MISATO;
						Dice.PlayerLocationY = 15 + MISATO;
						break;
					case 5:
						Dice.charY = -TANAKA;
						Dice.PlayerLocationY = 15 + TANAKA;
						break;
					default:
						Dice.charY = -UEDA;
						Dice.PlayerLocationY = 15 + UEDA;
						break;
			
					}
				}
				else if (Dice.mapstate == 1)
				{
					switch (Dice.dice)
					{
					case 0:
						Dice.charY = -IMAI;
						Dice.PlayerLocationY = 15 + IMAI;
						break;
					case 1:
					case 4:
						Dice.charY = -HIJIRI;
						Dice.PlayerLocationY = 15 + HIJIRI;
						break;
					case 2:
					case 5:
						Dice.charY = -MATUMOTO;
						Dice.PlayerLocationY = 15 + MATUMOTO;
						break;
					default:
						Dice.charY = -HIMEKAWA;
						Dice.PlayerLocationY = 15 + HIMEKAWA;
						break;
					}
				}
				else if (Dice.mapstate == 2)
				{
					switch (Dice.dice)
					{
					case 0:
						Dice.charY = -YOKOYAMA;
						Dice.PlayerLocationY = 15 + YOKOYAMA;
						break;
					case 1:
						Dice.charY = -KOMATU;
						Dice.PlayerLocationY = 15 + KOMATU;
						break;
					case 2:
						Dice.charY = -NANJO;
						Dice.PlayerLocationY = 15 + NANJO;
						break;
					case 4:
						Dice.charY = -MAKINO;
						Dice.PlayerLocationY = 15 + MAKINO;
						break;
					case 5:
						Dice.charY = -TAKATUKI;
						Dice.PlayerLocationY = 15 + TAKATUKI;
						break;
					case 6:
						Dice.charY = -KISHIBE;
						Dice.PlayerLocationY = 15 + KISHIBE;
						break;
					default:
						Dice.charY = -TATIBANA;
						Dice.PlayerLocationY = 15 + TATIBANA;
						break;
					}
				}
				Dice.HP--;
				Dice.turn++;
				Dice.extend = 1;
				Dice.state = 0;
				Dice.angelstate = 0;
				time = 0;
			}
			
		}
		break;
	}
}
void happening() {
	switch (Dice.dice)
	{
	case 0:
		DrawString(150, 200, "HPドリンクゲット!", GreenB);
		break;
	case 1:
		DrawString(150, 200, "5000円を拾った!", GreenB);
		break;
	case 2:
		DrawString(150, 200, "HPが3消費した!!", GreenB);
		break;
	case 3:
		DrawString(150, 200, "HPが2回復した!", GreenB);
		break;
	case 4:
		DrawString(150, 200, "1000円を拾った!", GreenB);
		break;
	case 5:
		DrawString(150, 200, "HPが半分に!!", GreenB);
		break;
	case 6:
		DrawString(150, 200, "次の出目は1だ!!", GreenB);
		break;
	case 7:
		DrawString(150, 200, "10000円を拾った!", GreenB);
		break;
	case 8:
		DrawString(150, 200, "所持金が2倍に!!!", GreenB);
		break;
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		Dice.dicedecisionA = 15 - Dice.dice;
		DrawFormatString(150, 200, GreenB, "%dマス戻る!!", Dice.dicedecisionA);
		break;
	}
}
void angel() {
	if (Dice.mapstate == 0)
	{
		switch (Dice.dice)
		{
		case 0:
			DrawString(170, 200, "中野駅に導かれよ!", GreenB);
			break;
		case 1:
			DrawString(170, 200, "日野駅に導かれよ!", GreenB);
			break;
		case 2:
			DrawString(170, 200, "大月駅に導かれよ!", GreenB);
			break;
		case 3:
			DrawString(170, 200, "甲斐大和駅に導かれよ!", GreenB);
			break;
		case 4:
			DrawString(170, 200, "美里駅に導かれよ!", GreenB);
			break;
		case 5:
			DrawString(170, 200, "田中駅に導かれよ!", GreenB);
			break;
		default:
			DrawString(170, 200, "上田駅に導かれよ!", GreenB);
			break;
		}
	}
	else if (Dice.mapstate == 1)
	{
		switch (Dice.dice)
		{
		case 0:
			DrawString(170, 200, "今井駅に導かれよ!", GreenB);
			break;
		case 1:
		case 4:
			DrawString(170, 200, "聖高原駅に導かれよ!", GreenB);
			break;
		case 2:
		case 5:
			DrawString(170, 200, "松本駅に導かれよ!", GreenB);
			break;
		default:
			DrawString(170, 200, "姫川駅に導かれよ!", GreenB);
			break;
		}
	}
	else if (Dice.mapstate == 2)
	{
		switch (Dice.dice)
		{
		case 0:
			DrawString(170, 200, "横山駅に導かれよ!", GreenB);
			break;
		case 1:
			DrawString(170, 200, "小松駅に導かれよ!", GreenB);
			break;
		case 2:
			DrawString(170, 200, "南条駅に導かれよ!", GreenB);
			break;
		case 4:
			DrawString(170, 200, "マキノ駅に導かれよ!", GreenB);
			break;
		case 5:
			DrawString(170, 200, "高槻駅に導かれよ!", GreenB);
			break;
		case 6:
			DrawString(170, 200, "岸辺駅に導かれよ!", GreenB);
			break;
		default:
			DrawString(170, 200, "立花駅に導かれよ!", GreenB);
			break;
		}
	}
	
}

void screenmove() {
	switch (state)
	{
	case TITLE:
		if (Key[KEY_INPUT_SPACE] == 1)
		{
			state = MENU;
		}
		if (Key[KEY_INPUT_I] == 1 && fdata.savedata == 0)//セーブデータ読み込み
		{
			fp = fopen(filename, "rb");
			if (fp == NULL) {
				fdata.savedata = 2;
			}
			else {
				fread(&fdata, sizeof(fdata), 1, fp);
				fclose(fp);
				fdata.savedata = 1;
			}
		}
		break;
	case MENU:
		if (Key[KEY_INPUT_UP] == 1)//難易度選択
		{
			if (Menu.level == 2)
			{
				Menu.level = 0;
			}
			else
			{
				Menu.level++;
			}
		}
		if (Key[KEY_INPUT_RIGHT] == 1)//ゲームスピード
		{
			if (Dice.speed == 3)
			{
				Dice.speed = 1;
			}
			else {
				Dice.speed++;
			}
		}
		
		if (Key[KEY_INPUT_S] == 1)//ゲームスタート
		{
			diceinit();
			Dice.state = 0;
			Dice.charnum = 0;
			Dice.PlayerLocationY = 15;
			Dice.extend = 1;
			fdata.togoal = false;
			state = DICE;
		}
		if (Key[KEY_INPUT_T] == 1)//タイトル戻り
		{
			state = TITLE;
		}
		if (Key[KEY_INPUT_N] == 1 && fdata.savedata == 1 && fdata.togoal == false)//ファイル記憶値をもとにデータ復旧,ゴールした直後は選ばせない
		{
			PlayMusic("bgm8.ogg", DX_PLAYTYPE_LOOP);
			Dice.state = 0;

			//ファイルから読み込み部→ゲームデータに反映
			Dice.turn = fdata.turn;
			Dice.charY = fdata.charY;
			Dice.nextone = fdata.nextone;
			Dice.speed = fdata.speed;

			Dice.charnum = fdata.charnum;
			Dice.PlayerLocationY = fdata.PlayerLocationY;
			Dice.extend = fdata.extend;
			Dice.mapstate = fdata.mapstate;

			Dice.HP = fdata.HP;
			Dice.HPdrink = fdata.HPdrink;
			Dice.money = fdata.money;
			Dice.rapidcard = fdata.rapidcard;
			Dice.expresscard = fdata.expresscard;

			time = 0;
			state = DICE;
		}
		break;
	case DICE:
		switch (Dice.state)
		{
		case 0:
			goalcheck();
			if (Dice.HP<=0)//ゲームオーバー判定
			{
				if (Key[KEY_INPUT_T] == 1)
				{
					Dice.charnum = 0;
					Dice.PlayerLocationY = 15;
					Dice.charY = 0;
					Dice.extend = 1;
					Dice.state = 0;
					Dice.mapstate = 1;
					init();
				}
			}
			else//メニュー
			{
				if (Key[KEY_INPUT_A] == 1)
				{
					Dice.state = 3;
					fdata.saveOK = false;//フラグ解除
				}
				if (Key[KEY_INPUT_B] == 1)
				{
					Dice.getmoney = 1000 + GetRand(200) * 10;
					Dice.money += Dice.getmoney;
					Dice.state = 1;
					fdata.saveOK = false;//フラグ解除
				}
				if (Key[KEY_INPUT_C] == 1)
				{
					Dice.state = 2;
					fdata.saveOK = false;//フラグ解除
				}
				if (Key[KEY_INPUT_D] == 1)
				{
					Dice.state = 5;
					fdata.saveOK = false;//フラグ解除
				}
				if (Key[KEY_INPUT_E] == 1)//データセーブ
				{
					fdata.turn = Dice.turn;
					fdata.charY = Dice.charY;
					fdata.nextone = Dice.nextone;
					fdata.speed = Dice.speed;

					fdata.charnum = Dice.charnum;
					fdata.PlayerLocationY = Dice.PlayerLocationY;
					fdata.extend = Dice.extend;
					fdata.mapstate = Dice.mapstate;

					fdata.HP = Dice.HP;
					fdata.HPdrink = Dice.HPdrink;
					fdata.money = Dice.money;
					fdata.rapidcard = Dice.rapidcard;
					fdata.expresscard = Dice.expresscard;
					fp = fopen(filename, "wb");
					if (fp != NULL) {
						fwrite(&fdata, sizeof(fdata), 1, fp);
						fclose(fp);
						fdata.saveOK = true;
					}
				}
			}
			break;
		case 1://バイト
			if (Key[KEY_INPUT_C] == 1)
			{
				Dice.state = 0;
				Dice.turn++;
				Dice.HP--;
			}
			break;
		case 2://アイテム使用
			if (Key[KEY_INPUT_A] == 1 && Dice.HPdrink > 0)
			{
				Dice.HP += GetRand(9) + 1;
				Dice.HPdrink--;
				Dice.state = 0;
			}
			if (Key[KEY_INPUT_B] == 1 && Dice.rapidcard > 0)
			{
				Dice.extend = 2;
				Dice.rapidcard--;
				Dice.state = 0;
			}
			if (Key[KEY_INPUT_C] == 1 && Dice.expresscard > 0)
			{
				Dice.extend = 4;
				Dice.expresscard--;
				Dice.state = 0;
			}
			if (Key[KEY_INPUT_M] == 1)
			{
				Dice.state = 0;
			}
			break;

		case 3://ダイス振り
			if (Key[KEY_INPUT_S] == 1)
			{
				if (Dice.nextone==true)//1だけしか進めないトラップ発動時
				{
					Dice.dicedecision = 1;
					Dice.nextone = false;//トラップフラグ解除
				}
				else
				{
					Dice.dicedecision = Dice.dice*Dice.extend;//サイコロの目を確認させる
					
				}
				Dice.state = 4;
			}
			break;
		case 4://進行
			time++;
			if (time % Dice.speed == 0)
			{
				if (Dice.dicedecision > 0)//1マス進み
				{
					Dice.charY -= 60;
					Dice.PlayerLocationY += 60;
					Dice.dicedecision--;
					goalcheck();
				}
				else
				{
					time = 0;
					Dice.happening = GetRand(7);
					if (Dice.mapstate == 0)
					{
						switch (Dice.charnum - 8)//14,26,29,71,77,80
						{
						case HINO / 60:
						case OTSUKI / 60:
						case KAIYAMATO / 60:
						case MISATO / 60:
						case TANAKA / 60:
						case UEDA / 60:
							Dice.state = ANGEL;
							break;
						default:
							Dice.extend = 1;
							if (!Dice.happening)//ハプニングタイムあり
							{	
								Dice.state = HAPPEN;
							}
							else//ハプニングタイムなし→次のターン
							{
								Dice.HP--;//hp消費
								Dice.turn++;
								Dice.state = 0;
							}
							break;
						}
					}
					else if (Dice.mapstate == 1)//11,28,49,62,65
					{
						switch (Dice.charnum - 8)
						{
						case HIJIRI / 60:
						case MATUMOTO / 60:
						case HIMEKAWA / 60:
							Dice.state = ANGEL;
							break;
						default:
							Dice.extend = 1;
							if (!Dice.happening)//ハプニングタイムあり
							{	
								Dice.state = HAPPEN;
							}
							else//ハプニングタイムなし→次のターン
							{
								Dice.HP--;
								Dice.turn++;
								Dice.state = 0;
							}
							break;
						}
					}
					else if (Dice.mapstate == 2)
					{
						switch (Dice.charnum - 8)
						{
						case KOMATU / 60:
						case NANJO / 60:
						case MAKINO / 60:
						case TAKATUKI / 60:
						case KISHIBE / 60:
						case TATIBANA / 60:
							Dice.state = ANGEL;
							break;
						default:
							Dice.extend = 1;
							if (!Dice.happening)//ハプニングタイムあり
							{
								Dice.state = HAPPEN;
							}
							else//ハプニングタイムなし→次のターン
							{
								Dice.HP--;
								Dice.turn++;
								Dice.state = 0;
							}
							break;
						}
					}
				}
			}
		    break;

		case GOAL:
			if (Key[KEY_INPUT_T] == 1)
			{
				if (Dice.mapstate == 0)
				{
					Dice.HP += GetRand(19) + 1;
					Dice.charnum = 0;
					Dice.PlayerLocationY = 15;
					Dice.charY = 0;
					Dice.extend = 1;
					Dice.state = 0;
					Dice.mapstate = 1;//nextstage go
				}
				else if (Dice.mapstate == 1)
				{
					Dice.HP += GetRand(19) + 1;
					Dice.charnum = 0;
					Dice.PlayerLocationY = 15;
					Dice.charY = 0;
					Dice.extend = 1;
					Dice.state = 0;
					Dice.mapstate = 2;//nextstage go
				}
				else
				{
					if (Menu.level == 0)//甘口
					{
						fdata.amacount++;//甘口クリア回数アップ
					}
					else if (Menu.level == 1)//中辛
					{
						fdata.tyucount++;//中辛クリア回数アップ
					}
					else//辛口
					{
						fdata.karacount++;//辛口クリア回数アップ
					}
					fdata.togoal = true;
					//自動的にセーブする
					fp = fopen(filename, "wb");
					if (fp != NULL) {
						fwrite(&fdata, sizeof(fdata), 1, fp);
						fclose(fp);
					}
					init();
				}

			}
			break;

		case 5://買い物
			if (Key[KEY_INPUT_A] == 1 && Dice.money >= 5000)
			{
				Dice.money -= 5000;
				Dice.HPdrink++;
				Dice.state = 0;
			}
			if (Key[KEY_INPUT_B] == 1 && Dice.money >= 10000)
			{
				Dice.money -= 10000;
				Dice.rapidcard++;
				Dice.state = 0;
			}
			if (Key[KEY_INPUT_C] == 1 && Dice.money >= 20000)
			{
				Dice.money -= 20000;
				Dice.expresscard++;
				Dice.state = 0;
			}
			if (Key[KEY_INPUT_M] == 1)
			{
				Dice.state = 0;
			}
			break;

		case HAPPEN:
			if (Key[KEY_INPUT_S] == 1)
			{
				Dice.dicedecision = Dice.dice;
				Dice.dicedecisionA = 15 - Dice.dice;
				Dice.happenstate = 1;
			}
			break;

		case ANGEL:
			if (Key[KEY_INPUT_S] == 1)
			{
				Dice.dicedecision = Dice.dice;
				Dice.angelstate = 1;
			}
			break;
		}
		break;
	 }
  }

void map() {
	SetFontSize(25);
	
	if (Dice.turn % 5 == 0)
	{

		DrawBox(0, 0, 295, 480, GetColor(0, 0, 57), TRUE);
	}
	else if (Dice.turn % 5 == 1)
	{
		DrawBox(0, 0, 295, 480, GetColor(66, 0, 122), TRUE);
	}
	else if (Dice.turn % 5 == 2)
	{
		DrawBox(0, 0, 295, 480, GetColor(156, 167, 22), TRUE);
	}
	else if (Dice.turn % 5 == 3)
	{
		DrawBox(0, 0, 295, 480, GetColor(250, 167, 222), TRUE);
	}
	else
	{
		DrawBox(0, 0, 295, 480, GetColor(200, 74, 25), TRUE);
	}

	for (Dice.i = 15; Dice.i <= Dice.charY + 60 * pointlim(Dice.mapstate) + 10; Dice.i += 60)
	{
		DrawCircle(70, Dice.i, 10, GreenA, TRUE);
		for (Dice.charnum = (Dice.PlayerLocationY-15) / 60; Dice.charnum <= limited(Dice.charnum) + Dice.PlayerLocationY / 60; Dice.charnum++)
		{
			if (Dice.mapstate == 0)
			{
				switch (Dice.charnum)
				{
				case HINO/60:
				case OTSUKI/60:
				case KAIYAMATO/60:
				case MISATO/60:
				case TANAKA/60:
				case UEDA/60:
					DrawFormatString(85, Dice.charY + 60 * Dice.charnum, orange, "%s", StationOfTohokuline[Dice.charnum]);
					break;
				default:
					DrawFormatString(85, Dice.charY + 60 * Dice.charnum, GreenA, "%s", StationOfTohokuline[Dice.charnum]);
					break;
				}
			}

			else if (Dice.mapstate == 1)
			{
				switch (Dice.charnum)
				{
				case HIJIRI/60:
				case MATUMOTO/60:
				case HIMEKAWA/60:
					DrawFormatString(85, Dice.charY + 60 * Dice.charnum, orange, "%s", StationOfTokaidoline[Dice.charnum]);
					break;
				default:
					DrawFormatString(85, Dice.charY + 60 * Dice.charnum, GreenA, "%s", StationOfTokaidoline[Dice.charnum]);
					break;
				}
			}

			else if (Dice.mapstate == 2)
			{
				switch (Dice.charnum)
				{
				case KOMATU/60:
				case NANJO/60:
				case MAKINO/60:
				case TAKATUKI/60:
				case KISHIBE/60:
				case TATIBANA/60:
					DrawFormatString(85, Dice.charY + 60 * Dice.charnum, orange, "%s", StationOfMountline[Dice.charnum]);
					break;
				default:
					DrawFormatString(85, Dice.charY + 60 * Dice.charnum, GreenA, "%s", StationOfMountline[Dice.charnum]);
					break;
				}
			}

		}
	}
	DrawCircle(70, 15, 11, red, TRUE);
}
void status() {
	SetFontSize(24);
	
	for (int i = 635; i <= 640; i++)
	{
		for (int j = 290; j <= 295; j++)
		{
			for (int k = 0; k <= 10; k++)
			{
				for (int l = 470; l <=480; l++)
				{
					DrawBox(j, k, i, l, white, FALSE);
				}
			}
			
		}
		
	}
	for (float m = 166.0f; m <= 174.0f; m++)
	{
		DrawLineAA(295.0f, m, 640.0f, 170.0f, white);
		DrawLineAA(295.0f, m+170.0f, 640.0f, 340.0f, white);
	}
	
	

	DrawFormatString(300, 20, GreenB, "HP:%d", Dice.HP);
	
	DrawFormatString(300, 50, GreenB, "HPドリンク:%d個", Dice.HPdrink);
	DrawFormatString(300, 80, GreenB, "所持金:%d", Dice.money);
	DrawFormatString(300, 110, GreenB, "快速カード:%d枚", Dice.rapidcard);
	DrawFormatString(300, 140, GreenB, "急行カード:%d枚", Dice.expresscard);
	DrawString(300, 350, "今回の道のり", GreenB);
	if (Dice.mapstate == 0) {
		DrawString(300, 380, "Part1:中野→今井", red);
		DrawString(300, 410, "Part2:今井→横山", GreenB);
		DrawString(300, 440, "Part3:横山→大石", GreenB);
	}
	else if (Dice.mapstate == 1) {
		DrawString(300, 380, "Part1:中野→今井", GreenB);
		DrawString(300, 410, "Part2:今井→横山", red);
		DrawString(300, 440, "Part3:横山→大石", GreenB);
	}
	else if (Dice.mapstate == 2) {
		DrawString(300, 380, "Part1:中野→今井", GreenB);
		DrawString(300, 410, "Part2:今井→横山", GreenB);
		DrawString(300, 440, "Part3:横山→大石", red);
	}
}
	
void boxcolor() {
	time++;
	DrawBox(0, 0, time % 640, 60, GreenB, true);
	DrawBox(0, 420, 640 - (time % 640), 480, yellow, true);
	DrawBox(80, 0, 150, time % 480, skyblue, true);
	DrawBox(460, 480 - (time % 480), 530, 480, white, true);
}

void goalcheck() {
	if (Dice.charY == -(ONEMAPNUM - 1)*60 && Dice.mapstate == 0 || Dice.charY == -(TWOMAPNUM - 1) * 60 && Dice.mapstate == 1
		|| Dice.charY == -(THREEMAPNUM - 1) * 60 && Dice.mapstate == 2)//ゴール判定
	{
		time = 0;
		Dice.state = GOAL;
	}
}