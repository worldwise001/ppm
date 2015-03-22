import ppm.Fraction;
import ppm.PPM;
import ppm.Trie;


public class Main {

	public static void main(String[] args) {
		//String test = "assanissimassa";
		String test = "Kenkichi Oshima (大島 鎌吉 Ōshima Kenkichi?, November 10, 1908 – March 30, 1985) was a Japanese Olympic medalist in athletics at the 1932 Summer Olympics in Los Angeles."
						+ "Ōshima was born in Kanazawa, Ishikawa Prefecture, Japan. While still a student, he set a new unofficial world record for the triple jump, and was widely expected to secure the gold medal in the 1932 Los Angeles Olympics. However, Ōshima suffered from severe burns in a bathing accident shortly before the start of the Olympics, and was not in the best form for the competition. However, he was still able to secure the bronze medal for the triple jump event with a distance of 15.12 meters. Fellow teammate Chuhei Nambu took the gold medal in this event. Ōshima also participated in the same event in the 1936 Summer Olympics in Berlin, but came in sixth place, while teammates Naoto Tajima and Masao Harada took the gold and silver medals."
						+ "Fluent in the German language, during the Berlin Olympics, Ōshima had a 20 minute interview of Adolf Hitler, who apparently mistook Ōshima for a Japanese military attache of the same surname. Ōshima asked Hitler many questions on Nazi initiatives to improve on the physical education of German youth."
						+ "After graduation from Kansai University, Ōshima was employed by the Mainichi Shimbun, working as a reporter in the sports section of the newspaper until retirement. He then accepted the post of professor emeritus (and later Vice President) of Osaka University of Health and Sport Sciences, where he undertook reforms of the athletic department to strengthen the Japanese team in preparation for the 1964 Tokyo Olympics. He was also an honorary member of the International Olympic Committee."
						+ "Ōshima was also author of numerous books on sports.";
		int charSet = Trie.calculateCharacterProbability(test);
		
		Trie trie = new Trie(5, PPM.Type.PPMC, new Fraction(1L, (long)charSet));
		test = test.toLowerCase();
		for (int i = 0; i < test.length(); i++) {
			trie.add(new Character(test.charAt(i)));
			Fraction probability = trie.getProbabilityEncoding();
			double bits = trie.getBitEncoding();
			System.out.printf("%c encoded with probability %d/%d, or %5.2f bits\n",
					test.charAt(i),
					probability.x,
					probability.y,
					bits);
		}
	}
}
