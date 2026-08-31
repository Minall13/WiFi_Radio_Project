import processing.svg.*;
import java.io.*;

// =====================================================
// SVG to RAW565 Icon Asset Builder
// Embedded Project Framework
// =====================================================

String inputFolder  = "icons";
String outputFolder = "output";

final int MAX_ICON_SIZE = 52;
int[] iconSizes = { 24, 32, 40, 48, 52 };
int generatedCount = 0;

IconTheme[] themes = {
  new IconTheme("white",  255, 255, 255),
  new IconTheme("black",    0,   0,   0),
  new IconTheme("blue",     0,  90, 220),
  new IconTheme("green",    0, 255,   0),
  new IconTheme("amber",  255, 160,   0),
  new IconTheme("red",    255,   0,   0),
  new IconTheme("yellow", 255, 220,   0)
};

IconJob[] jobs = {
  new IconJob("wifi_on",    "android_wifi_3_bar_48dp_1F1F1F_FILL0_wght500_GRAD200_opsz48.svg"),
  new IconJob("wifi_off",   "android_wifi_3_bar_off_48dp_1F1F1F_FILL0_wght500_GRAD200_opsz48.svg"),
  new IconJob("stations",   "bookmark_stacks_48dp_1F1F1F_FILL0_wght500_GRAD200_opsz48.svg"),
  new IconJob("catalogue",  "import_contacts_48dp_1F1F1F_FILL0_wght500_GRAD200_opsz48.svg"),
  new IconJob("router_on",  "router_48dp_1F1F1F_FILL0_wght500_GRAD200_opsz48.svg"),
  new IconJob("router_off", "router_off_48dp_1F1F1F_FILL0_wght500_GRAD200_opsz48.svg"),
};

void settings() {
  size(MAX_ICON_SIZE, MAX_ICON_SIZE, P2D);
  pixelDensity(1);
}

void setup() {
  println("====================================");
  println("SVG to RAW565 Icon Asset Builder");
  println("v1.1");
  println("Generating " + jobs.length + " icons");
  println(iconSizes.length + " sizes");
  println(themes.length + " colour themes");
  println("====================================");

  for (int j = 0; j < jobs.length; j++) {
    for (int t = 0; t < themes.length; t++) {
      for (int s = 0; s < iconSizes.length; s++) {
        generateIcon(jobs[j], themes[t], iconSizes[s]);
      }
    }
  }

  println();
  println("====================================");
  println("SVG to RAW565 Icon Asset Builder");
  println("v1.1");
  println("Generating " + jobs.length + " icons");
  println(iconSizes.length + " sizes");
  println(themes.length + " colour themes");
  println("------------------------------------");
  println(generatedCount + " RAW565 assets generated.");
  println("Output: " + outputFolder + "/");
  println("====================================");

  exit();
}

void generateIcon(IconJob job, IconTheme theme, int iconSize) {
  println("Generating: " + job.name + "_" + theme.name + "_" + iconSize + ".raw");

  PShape icon = loadShape(inputFolder + "/" + job.sourceFile);

  if (icon == null) {
    println("ERROR: Cannot load SVG: " + job.sourceFile);
    return;
  }

  background(0, 0);
  shapeMode(CORNER);

  int inset = 2;  // Draw 20×20 artwork centred within a 24×24 icon
  shape(icon, inset, inset, iconSize - (inset * 2), iconSize - (inset * 2));

  String outputPath =
    outputFolder + "/" + job.name + "_" + theme.name + "_" + iconSize + ".raw";

  saveRGB565(outputPath, iconSize, iconSize, theme.colour);
}

void saveRGB565(String filename, int w, int h, color iconColour) {
  loadPixels();

  try {
    File outFile = new File(sketchPath(filename));
    outFile.getParentFile().mkdirs();

    DataOutputStream out =
      new DataOutputStream(new FileOutputStream(outFile));

    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        color c = pixels[y * width + x];

        if (alpha(c) > 0) {
          c = iconColour;
        }

        int r = (int)red(c);
        int g = (int)green(c);
        int b = (int)blue(c);

        int rgb565 =
          ((r & 0xF8) << 8) |
          ((g & 0xFC) << 3) |
          (b >> 3);

        out.write(rgb565 >> 8);
        out.write(rgb565 & 0xFF);
      }
    }

    out.close();

    generatedCount++;

  }
  catch (Exception e) {
    println("ERROR writing: " + filename);
    e.printStackTrace();
  }
}

class IconTheme {
  String name;
  color colour;

  IconTheme(String name, int r, int g, int b) {
    this.name = name;
    this.colour = color(r, g, b);
  }
}

class IconJob {
  String name;
  String sourceFile;

  IconJob(String name, String sourceFile) {
    this.name = name;
    this.sourceFile = sourceFile;
  }
}
