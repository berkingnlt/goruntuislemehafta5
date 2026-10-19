#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

// Siyah -> kırmızı -> sarı -> beyaz paleti.
// Girdi tek kanallı gri, çıktı BGR renkli görüntüdür.
Mat kendiRenklendir(const Mat& gri)
{
    Mat renkli(gri.size(), CV_8UC3);

    for (int y = 0; y < gri.rows; y++)
    {
        for (int x = 0; x < gri.cols; x++)
        {
            int deger = gri.at<uchar>(y, x);

            int B = 0;
            int G = 0;
            int R = 0;

            if (deger < 85)
            {
                // Siyah -> kırmızı
                R = deger * 3;
            }
            else if (deger < 170)
            {
                // Kırmızı -> sarı
                R = 255;
                G = (deger - 85) * 3;
            }
            else
            {
                // Sarı -> beyaz
                R = 255;
                G = 255;
                B = (deger - 170) * 3;
            }

            renkli.at<Vec3b>(y, x) = Vec3b(
                static_cast<uchar>(B),
                static_cast<uchar>(G),
                static_cast<uchar>(R)
            );
        }
    }

    return renkli;
}

// Orijinal ve renklendirilmiş görüntü arasındaki fark.
void farkHesapla(
    const Mat& orijinal,
    const Mat& renkli,
    const string& dosyaAdi)
{
    unsigned long long toplamFark = 0;

    for (int y = 0; y < orijinal.rows; y++)
    {
        for (int x = 0; x < orijinal.cols; x++)
        {
            Vec3b a = orijinal.at<Vec3b>(y, x);
            Vec3b b = renkli.at<Vec3b>(y, x);

            for (int kanal = 0; kanal < 3; kanal++)
            {
                int fark =
                    static_cast<int>(a[kanal]) -
                    static_cast<int>(b[kanal]);

                if (fark < 0)
                    fark = -fark;

                toplamFark += fark;
            }
        }
    }

    double ortalamaFark =
        static_cast<double>(toplamFark) /
        (static_cast<double>(orijinal.total()) * 3.0);

    cout << "\nDosya: " << dosyaAdi << endl;
    cout << "Orijinal / renklendirilmis mutlak fark toplami: "
        << toplamFark << endl;
    cout << "Ortalama mutlak fark: "
        << ortalamaFark << endl;
}

int main()
{
    string dosyalar[] = {
        "renkli.jpg",
        "renkli2.jpg",
        "renkli3.jpg",
        "renkli4.jpg",
        "renkli5.jpg"
    };

    int islenen = 0;

    for (int i = 0; i < 5; i++)
    {
        Mat orijinal = imread(dosyalar[i], IMREAD_COLOR);

        if (orijinal.empty())
        {
            cout << "Goruntu okunamadi: "
                << dosyalar[i] << endl;
            continue;
        }

        // 1. Önce tek kanallı gri görüntü oluştur.
        Mat gri;
        cvtColor(orijinal, gri, COLOR_BGR2GRAY);

        // 2. Gri görüntüyü kendi paletimizle renklendir.
        Mat renkli = kendiRenklendir(gri);

        // 3. Orijinal renkli görüntü ile karşılaştır.
        farkHesapla(orijinal, renkli, dosyalar[i]);

        string onEk = "gorev5_" + to_string(i + 1);

        imwrite(onEk + "_orijinal.png", orijinal);
        imwrite(onEk + "_gri.png", gri);
        imwrite(onEk + "_renkli.png", renkli);

        // Yan yana göstermek için griyi üç kanala genişlet.
        Mat griUcKanalli;
        cvtColor(gri, griUcKanalli, COLOR_GRAY2BGR);

        Mat parcalar[] = {
            orijinal,
            griUcKanalli,
            renkli
        };

        Mat karsilastirma;
        hconcat(parcalar, 3, karsilastirma);

        imwrite(onEk + "_karsilastirma.png", karsilastirma);

        namedWindow(onEk, WINDOW_NORMAL);
        imshow(onEk, karsilastirma);

        islenen++;
    }

    cout << "\nIslenen goruntu sayisi: " << islenen << "/5"
        << endl;

    if (islenen == 0)
        return -1;

    cout << "Solda orijinal, ortada gri, sagda renklendirilmis."
        << endl;

    waitKey(0);
    return 0;
}