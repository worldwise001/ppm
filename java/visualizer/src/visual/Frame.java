package visual;

import java.awt.GridLayout;
import java.util.ArrayList;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

import ppm.Fraction;
import ppm.PPM;
import ppm.Pair;
import ppm.Trie;

public class Frame extends JFrame {
	
	private GraphPanel gp;
	private TextColorPanel tcp;
	private ActionPanel ap;
	private Trie trie1;
	private Trie trie2;
	
	public Frame() {
		setup();
	}
	
	private void setup() {
		setTitle("Text Encoding Viewer");
		setSize(800,600);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		gp = new GraphPanel(this);
		tcp = new TextColorPanel(this);
		ap = new ActionPanel(this);
		
		GridLayout layout1 = new GridLayout(2,0);
		GridLayout layout2 = new GridLayout(2,0);
		
		setLayout(layout1);
		JPanel panel = new JPanel();
		panel.setLayout(layout2);
		
		add(ap);
		JScrollPane sp1 = new JScrollPane(tcp);
		add(sp1);
		//add(panel);
		//panel.add(tcp);
		//panel.add(gp);
	}
	
	public static void main(String[] args) {
		Frame f = new Frame();
		f.setVisible(true);
	}
	
	private void resetTries(int contextLength, long charSetSize) {
		trie1 = new Trie(contextLength, PPM.Type.PPMC, new Fraction(1L, charSetSize));
		trie2 = new Trie(contextLength, PPM.Type.PPMC, new Fraction(1L, charSetSize));
		System.gc();
	}
	
	private void train(Trie trie, String text) {
		double total = 0;
		for (int i = 0; i < text.length(); i++) {
			trie.add(new Character(text.charAt(i)));
			double bits = trie.getBitEncoding();
			total += bits;
		}
		System.out.printf("Train Compressibility: %.2f/%d or %.2f\n", total/8, text.length(), (total/8/text.length()));
		trie.printEscapes();
		System.out.println();
	}
	
	private Pair<Character, Pair<Fraction, Double>> testNextChar(Trie trie, char c) {
		Fraction p;
		double b;
		
		trie.add(c);
		p = trie.getProbabilityEncoding();
		b = trie.getBitEncoding();
		
		Pair<Fraction, Double> data = new Pair<Fraction, Double>(p, b);
		Pair<Character, Pair<Fraction, Double>> entry = new Pair<Character, Pair<Fraction, Double>>(c, data);
		return entry;
	}

	public void runAnalysis(String textTrain1, String textTrain2, String textTest, int cl, boolean cs) {
		if (!cs) {
			textTrain1 = textTrain1.toLowerCase();
			textTrain2 = textTrain2.toLowerCase();
			textTest = textTest.toLowerCase();
		}
		double t1 = 0, t2 = 0;
		double avgp = 0;
		int avgc = 0;
		
		long charSetSize = Trie.calculateCharacterProbability(textTrain1+textTrain2+textTest);
		System.out.println("Calculated character probability to be 1/"+charSetSize);
		resetTries(cl, charSetSize);
		
		train(trie1, textTrain1);
		train(trie2, textTrain2);
		
		trie1.clearEscapeArray();
		trie2.clearEscapeArray();
		
		ArrayList<Pair<Character, Double>> list = new ArrayList<Pair<Character, Double>>();
		
		for (int i = 0; i < textTest.length(); i++) {
			Pair<Character, Pair<Fraction, Double>> entry;
			char c = textTest.charAt(i);
			double p = 0;
			
			entry = testNextChar(trie1, c);
			t1 += entry.y.y;
			p -= 1.0*entry.y.x.x/entry.y.x.y;
			
			entry = testNextChar(trie2, c);
			t2 += entry.y.y;
			p += 1.0*entry.y.x.x/entry.y.x.y;
			
			avgp += p;
			avgc += (p>0)?1:((p<0)?-1:0);
			
			list.add(new Pair<Character, Double>(c, p));
		}
		System.out.printf("Test (trained1) Compressibility: %.2f/%d or %.2f\n", t1/8, textTest.length(), (t1/8/textTest.length()));
		trie1.printEscapes();
		System.out.println();
		System.out.printf("Test (trained2) Compressibility: %.2f/%d or %.2f\n", t2/8, textTest.length(), (t2/8/textTest.length()));
		trie2.printEscapes();
		System.out.println();
		System.out.printf("Avgp = %.2f,  Avgc = %d\n\n", avgp, avgc);
		
		Trie trie = new Trie(cl, PPM.Type.PPMC, new Fraction(1L, charSetSize));
		double total = 0;
		for (int i = 0; i < textTest.length(); i++) {
			trie.add(new Character(textTest.charAt(i)));
			double bits = trie.getBitEncoding();
			total += bits;
		}
		System.out.printf("Test (untrained) Compressibility: %.2f/%d or %.2f\n", total/8, textTest.length(), (total/8/textTest.length()));
		trie.printEscapes();
		System.out.println();
		
		tcp.update(list);
		gp.update(list);
		System.gc();
	}
	
}
