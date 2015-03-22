package visual;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

public class ActionPanel extends JPanel {
	
	private JTextArea textAreaTrain1;
	private JTextArea textAreaTrain2;
	private JTextArea textAreaTest;
	private JButton submit;
	private JTextField contextLength;
	private JComboBox<String> caseSensitivity;
	private Frame parent;
	
	public ActionPanel(Frame parent) {
		this.parent = parent;
		setup();
	}
	
	private void setup() {
		textAreaTrain1 = new JTextArea();
		textAreaTrain2 = new JTextArea();
		textAreaTest = new JTextArea();
		
		BorderLayout layout1 = new BorderLayout();
		setLayout(layout1);
		
		JPanel textPanel = new JPanel();
		GridLayout layout2 = new GridLayout(1,2);
		textPanel.setLayout(layout2);
		JScrollPane trainPane1 = new JScrollPane(textAreaTrain1);
		textPanel.add(trainPane1);
		JScrollPane trainPane2 = new JScrollPane(textAreaTrain2);
		textPanel.add(trainPane2);
		JScrollPane testPane = new JScrollPane(textAreaTest);
		textPanel.add(testPane);
		
		add(textPanel, BorderLayout.CENTER);
		
		JPanel buttonPanel = new JPanel();
		JLabel label1 = new JLabel("Context Length: ");
		contextLength = new JTextField(5);
		contextLength.setText("5");
		JLabel label2 = new JLabel("Case-sensitive: ");
		caseSensitivity = new JComboBox<String>(new String[]{"Y", "N"});
		
		submit = new JButton("Calculate");
		buttonPanel.add(label1);
		buttonPanel.add(contextLength);
		buttonPanel.add(label2);
		buttonPanel.add(caseSensitivity);
		buttonPanel.add(submit);
		
		add(buttonPanel, BorderLayout.SOUTH);
		
		submit.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				int cl = Integer.parseInt(contextLength.getText());
				boolean cs = (caseSensitivity.getSelectedItem().equals("Y"));
				parent.runAnalysis(textAreaTrain1.getText(), textAreaTrain2.getText(), textAreaTest.getText(), cl, cs);
			}
		});
	}

}
