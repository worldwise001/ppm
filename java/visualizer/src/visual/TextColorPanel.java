package visual;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.ArrayList;

import javax.swing.JPanel;

import ppm.Fraction;
import ppm.Pair;

public class TextColorPanel extends JPanel {
	
	private Frame parent;
	private int margin = 10;
	private int spacing = 0;
	private int fontSize = 14;
	private ArrayList<Pair<Character, Double>> list;
	
	public TextColorPanel(Frame parent) {
		this.parent = parent;
		setup();
	}
	
	private void setup() {
		list = new ArrayList<Pair<Character, Double>>();
	}

	@Override
	public void paint(Graphics g) {
		super.paint(g);
		Graphics2D g2 = (Graphics2D)g;
		g2.setBackground(Color.WHITE);
		g2.setColor(Color.WHITE);
		g2.fillRect(0, 0, getWidth(), getHeight());
		
		Font font = new Font("Monospaced", Font.BOLD, fontSize);
		g2.setFont(font);
		for (int i = 0; i < list.size(); i++) {
			Pair<Character,Double> entry = list.get(i);
			Character c = entry.x;
			Double d = entry.y;
			
			drawCharacter(g2, i, list.size(), c, d);
		}
	}

	private void drawCharacter(Graphics2D g2, int i, int size, Character c, double p) {
		FontMetrics fm = g2.getFontMetrics();
		//String s = String.format("%02x", (int)c);
		if (c == '\n') c = ' ';
		String s = c+"";
		int w = fm.stringWidth(s);
		if (w == 0)
			w = fm.stringWidth("m");
		int h = fm.getHeight();
		int sw = (getWidth()-(margin*2))/(w+spacing);
		int sh = (size/sw+2);
		int x = (i%sw)*(w+spacing);
		int y = (i/sw)*(h+spacing);
		
		this.setPreferredSize(new Dimension(0, sh*(h+spacing)));
		parent.repaint();
		
		float r = (float)((p<0)?1.0:(1-p));
		float g = (float)((p>0)?1.0:(1+p));
		float b = (float)(1-Math.abs(p));
		
		Color co = new Color(r, g, b);
		g2.setColor(co);
		g2.fillRect(x+margin, y+margin, w, h);
		g2.setColor(Color.black);
		g2.drawString(s, x+margin, y+fm.getAscent()+margin);
	}

	public void update(ArrayList<Pair<Character, Double>> list) {
		this.list = list;
		this.repaint();
	}
}
