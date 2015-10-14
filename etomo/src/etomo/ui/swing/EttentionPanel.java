package etomo.ui.swing;

import java.awt.Component;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.HashMap;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.text.JTextComponent;
import javax.xml.parsers.*;

import org.w3c.dom.*;
import org.xml.sax.SAXException;
import org.w3c.dom.traversal.DocumentTraversal;
import org.w3c.dom.traversal.NodeFilter;
import org.w3c.dom.traversal.TreeWalker;

import etomo.ApplicationManager;
import etomo.comscript.FortranInputSyntaxException;
import etomo.comscript.EttentionsetupParam;
import etomo.comscript.TiltParam;
import etomo.process.ImodManager;
import etomo.storage.LogFile;
import etomo.storage.EttentionOutputFileFilter;
import etomo.storage.autodoc.AutodocFactory;
import etomo.storage.autodoc.ReadOnlyAutodoc;
import etomo.type.AxisID;
import etomo.type.ConstEtomoNumber;
import etomo.type.ConstMetaData;
import etomo.type.DialogType;
import etomo.type.EtomoAutodoc;
import etomo.type.EtomoNumber;
import etomo.type.FileType;
import etomo.type.MetaData;
import etomo.type.PanelId;
import etomo.type.ReconScreenState;
import etomo.type.Run3dmodMenuOptions;
import etomo.type.StringParameter;
import etomo.type.TomogramState;
import etomo.ui.FieldType;
import etomo.ui.FieldValidationFailedException;

/**
* <p>Description: </p>
*
* <p>Copyright: Copyright 2010</p>
*
* <p>Organization:
* Boulder Laboratory for 3-Dimensional Electron Microscopy of Cells (BL3DEMC),
* University of Colorado</p>
*
* @author $Author$
*
* @version $Revision$
*
*/
final class EttentionPanel implements Run3dmodButtonContainer, EttentionsetupDisplay, Expandable,
    FieldObserver {
  public static final String rcsid = "$Id$";

  private final JPanel pnlRoot = new JPanel();

  private final ButtonGroup bgMethod = new ButtonGroup();

  private final RadioButton rbETSart = new RadioButton("SART", bgMethod);
  private final RadioButton rbETSirt = new RadioButton("SIRT", bgMethod);
  private final RadioTextField rbETOSSirt = new RadioTextField(FieldType.INTEGER, "OS-SIRT - Number of projections: ", bgMethod, null);
  //private final RadioTextField rbETPlugin = new RadioTextField(FieldType.STRING, "Plugin: ", bgMethod, null);
  private final RadioButton rbETPlugin = new RadioButton("Plugin: ", bgMethod);
  private final JComboBox cmbETPlugin = new JComboBox();

  private final CheckTextField ctfOversamplingForwardProjection = CheckTextField.getNumericInstance(
	      FieldType.INTEGER, "Oversampling in forward projection step: ",
	      EtomoNumber.Type.INTEGER);

  private final CheckTextField ctfOversamplingBackProjection = CheckTextField.getNumericInstance(
	      FieldType.INTEGER, "Oversampling in back projection step: ",
	      EtomoNumber.Type.INTEGER);

  private final CheckBox cbUseLongObjectCompensation = new CheckBox("Use long object compensation");

  private final LabeledTextField ltfNumberOfIterations = new LabeledTextField(
      FieldType.INTEGER, "Number of iterations: ");
  private final LabeledTextField ltfLambda = new LabeledTextField(
	      FieldType.FLOATING_POINT, "Relaxation parameter lambda: ");

  private final CheckBox cbSubarea = new CheckBox("Reconstruct subarea");
  private final LabeledTextField ltfSubareaSize = new LabeledTextField(
	      FieldType.STRING, "Subarea size: ");
  private final LabeledTextField ltfSubareaCenter = new LabeledTextField(
	      FieldType.STRING, "Subarea center: ");

  private HashMap<String, Object> extendedGUIParameter = new HashMap<String, Object>();

  private final ActionListener listener = new EttentionActionListener(this);
  private final Run3dmodButton btn3dmodEttention = Run3dmodButton.get3dmodInstance(
      "View Tomogram In 3dmod", this);

  private final SpacedPanel pnlEttentionsetupParamsBody = SpacedPanel.getInstance(true);
 // private final SpacedPanel pnlAvailablePluginsBody = SpacedPanel.getInstance(true);

  private final List<ResumeObserver> resumeObservers = new ArrayList();
  private final List<FieldObserver> fieldObservers = new ArrayList();
  private final List<JPanel> advancedPanels = new ArrayList();
  private final List<JPanel> pluginsPanels = new ArrayList();
  private final LinkedHashMap<String,String> pluginsNames = new LinkedHashMap<String, String>();

  private final AxisID axisID;
  private final ApplicationManager manager;
  private final Run3dmodButton btnEttention;
  private final MultiLineButton btnUseEttention;
  private final TomogramGenerationDialog parent;
  private final DialogType dialogType;
  private final PanelHeader ettentionSetupParamsHeader;
  //private final PanelHeader availablePluginsHeader;

  private int numFiles = 0;
  private boolean differentFromCheckpointFlag = false;

  private EttentionPanel(final ApplicationManager manager, final AxisID axisID,
      final DialogType dialogType, final GlobalExpandButton globalAdvancedButton,
      final TomogramGenerationDialog parent) {
    this.axisID = axisID;
    this.manager = manager;
    this.parent = parent;
    this.dialogType = dialogType;
    ProcessResultDisplayFactory factory = manager.getProcessResultDisplayFactory(axisID);
    btnEttention = (Run3dmodButton) factory.getEttentionsetup();
    btnUseEttention = (MultiLineButton) factory.getUseEttention();
    ettentionSetupParamsHeader = PanelHeader.getAdvancedBasicInstance("Ettention", this,
        dialogType);
   // availablePluginsHeader = PanelHeader.getInstance("Available Plugins", this,
    //        dialogType);
  }

  static EttentionPanel getInstance(final ApplicationManager manager, final AxisID axisID,
      final DialogType dialogType, final GlobalExpandButton globalAdvancedButton,
      final TomogramGenerationDialog parent) {
	  EttentionPanel instance = new EttentionPanel(manager, axisID, dialogType, globalAdvancedButton,
        parent);
    instance.createPanel();
    instance.setToolTipText();
    instance.addListeners();
    return instance;
  }

  Component getRoot() {
    return pnlRoot;
  }

  private void createExtendedPanel(JPanel pnlAdvanced, JPanel pnlPlugins)
  {
	  String basePathToConfigFiles = System.getenv("IMOD_DIR") + "/bin/plugins/ettention/gui";
	  addParametersFromFile( basePathToConfigFiles + "/Ettention.xml", pnlAdvanced, false );

	  File dir = new File( basePathToConfigFiles );
	  System.out.println("traversing directory " + basePathToConfigFiles );
	  File[] fileSystemChildren = dir.listFiles();
	  for ( int i = 0; i < fileSystemChildren.length; i++ )
	  {
		  File file = fileSystemChildren[i];
		  System.out.println("checking file ... " + fileSystemChildren[i] );
		  if ( ! file.isFile() )
		  {
			  System.out.println("... not file, skipping" );
			  continue;
		  }
		  if ( file.getName().equals("Ettention.xml" ) )
		  {
			  System.out.println("... default (already processed), skipping" );
			  continue;
		  }
		  if ( !file.getName().endsWith(".xml") )
		  {
			  System.out.println("... not GUI xml file, skipping" );
			  continue;
		  }
		  System.out.println("adding GUI for parameters from " + file.getAbsolutePath());
		  addParametersFromFile( file.getAbsolutePath(), pnlPlugins, true );
	  }
  }

  private void addParametersFromFile( String filename, JPanel parentPanel,  boolean isPluginGroup )
  {
	  DocumentBuilderFactory builderFactory = DocumentBuilderFactory.newInstance();
	  DocumentBuilder builder = null;
	  Document document = null;

	  try
	  {
		  builder = builderFactory.newDocumentBuilder();
		  document = builder.parse( new FileInputStream( filename ) );

		  NodeList groupList = document.getDocumentElement().getChildNodes();
		  for (int i = 0; i < groupList.getLength(); i++)
		  {
			  Node groupNode = groupList.item(i);
			  if ( groupNode.getNodeType() == Node.ELEMENT_NODE )
				  handleParameterGroup(groupNode, parentPanel, isPluginGroup );
		  }

	  } catch ( ParserConfigurationException e ) {
		  e.printStackTrace();
	  } catch ( SAXException e ) {
		  e.printStackTrace();
	  } catch ( IOException e ) {
		  e.printStackTrace();
	  }
  }

  private void handleParameterGroup( Node groupNode,  JPanel parentPanel, boolean isPluginGroup  )
  {
	  JPanel pnlGroup = new JPanel();

	  pnlGroup.setLayout( new BoxLayout(pnlGroup, BoxLayout.Y_AXIS) );
	  pnlGroup.setBorder(BorderFactory.createEtchedBorder());

	  Element groupElement = (Element) groupNode;

	  addCaptionToPanel(pnlGroup, groupElement.getAttribute("caption"), groupElement.getAttribute("id") );

	  NodeList groupList = groupNode.getChildNodes();
	  for (int i = 0; i < groupList.getLength(); i++)
	  {
		  Node parameterNode = groupList.item(i);
		  if ( parameterNode.getNodeType() == Node.ELEMENT_NODE )
		  {
			  String helpText = getHelpText(parameterNode);
			  Element parameterElement = (Element) parameterNode;
			  javax.swing.JComponent widget = generateWidgetForParameter(parameterNode);

			  addWidgetToPanel(pnlGroup, widget, parameterElement.getAttribute("caption"), helpText);

			  String parameterKey = groupElement.getTagName() + "." + parameterElement.getTagName();

			  extendedGUIParameter.put(parameterKey, widget);
		  }
	  }
	  parentPanel.add(pnlGroup);
	  if(isPluginGroup){
		  pluginsPanels.add(pnlGroup);
	  }
	  else{
		  advancedPanels.add(pnlGroup);
	  }
  }

  private String getHelpText(Node parameterNode)
  {
	  NodeList sublist = parameterNode.getChildNodes();
	  String helpText="";
	  for (int j = 0; j < sublist.getLength(); j++)
	  {
		  Node subNode = sublist.item(j);
		  if(subNode.getNodeName().equals("helptext"))
		  {
			  helpText = subNode.getTextContent();
			  break;
		  }
	  }

	  return helpText;
  }

  private void addCaptionToPanel(JPanel pnlGroup, String caption,  String id)
  {
	  String captionWithID = "  " + caption;
	  if(!id.isEmpty()){
		  pluginsNames.put(caption,id);
	  }

	  JPanel pnlCaption = new JPanel();
	  pnlCaption.setLayout( new BoxLayout(pnlCaption, BoxLayout.Y_AXIS) );
	  pnlCaption.setAlignmentX(Box.CENTER_ALIGNMENT);
	  pnlCaption.add(Box.createRigidArea(FixedDim.x0_y5));
	  pnlCaption.add( new JLabel( captionWithID ) );
	  pnlCaption.add(Box.createRigidArea(FixedDim.x0_y5));
	  pnlCaption.add(Box.createHorizontalGlue());
	  pnlGroup.add( pnlCaption );
  }

  private void addWidgetToPanel(JPanel pnlGroup, javax.swing.JComponent widget, String caption, String helpText)
  {
	  JPanel pnlParameter = new JPanel();

	  if ( widget instanceof JTextField )
	  {
		  pnlParameter.setLayout( new BoxLayout(pnlParameter, BoxLayout.X_AXIS) );
		  JLabel lblCaption = new JLabel("  " + caption + ":  ");
		  pnlParameter.add( lblCaption);
		  lblCaption.setToolTipText("<html><p width=\"350px\">" + helpText + "</p></html>");
		  pnlParameter.add( widget );
	  }
	  else if ( widget instanceof JCheckBox )
	  {
		  pnlParameter.setLayout( new BoxLayout(pnlParameter, BoxLayout.Y_AXIS) );
		  pnlParameter.setAlignmentX(Box.CENTER_ALIGNMENT);
		  pnlParameter.add( widget );
		  pnlParameter.add(Box.createHorizontalGlue());
	  }

	  widget.setToolTipText("<html><p width=\"350px\">" + helpText + "</p></html>");
	  pnlGroup.add( pnlParameter );
	  pnlGroup.add(Box.createHorizontalGlue());
  }

  private javax.swing.JComponent generateWidgetForParameter( Node parameterNode )
  {
	  Element parameterElement = (Element) parameterNode;
	  if ( parameterElement.getAttribute("type").equals("boolean") )
	  {
		  JCheckBox checkBox = new JCheckBox(parameterElement.getAttribute("caption"));
		  checkBox.setSelected( parameterElement.getAttribute("default").equals("true") );
		  return checkBox;
	  }

	  JTextField textField = new JTextField();
	  textField.setText( parameterElement.getAttribute("default") );
	  return textField;
  }

  private void createPanel() {
    // initialize
	JPanel pnlSubarea = new JPanel();
	JPanel pnlSubareaCB = new JPanel();
	JPanel pnlSubareaSize = new JPanel();
	JPanel pnlCenter = new JPanel();
    JPanel pnlEttentionsetupParams = new JPanel();
    JPanel pnlButtons = new JPanel();
    JPanel pnlETmethod1 = new JPanel();
    JPanel pnlBasicOptions = new JPanel();
    JPanel pnlUseLongObjectCompensation = new JPanel();
    JPanel pnlOversamplingOptions = new JPanel();
    JPanel pnlAdvancedParams = new JPanel();
    JPanel pnlPluginsContainer = new JPanel();
    JPanel pnlETmethod2 = new JPanel();
    JPanel pnlAvailablePlugins = new JPanel();
    JPanel pnlAvailablePluginsLabel = new JPanel();
    SpacedPanel pnlAvailablePluginsBody = SpacedPanel.getInstance(true);
    btnEttention.setSize();
    btnEttention.setContainer(this);
    btnEttention.setDeferred3dmodButton(btn3dmodEttention);
    btn3dmodEttention.setSize();
    btnUseEttention.setSize();
    // root panel
    pnlRoot.setLayout(new BoxLayout(pnlRoot, BoxLayout.Y_AXIS));
    pnlRoot.add(pnlEttentionsetupParams);
    pnlRoot.add(Box.createRigidArea(FixedDim.x0_y10));
    pnlRoot.add(pnlAvailablePlugins);
    pnlRoot.add(Box.createRigidArea(FixedDim.x0_y10));
    pnlRoot.add(pnlButtons);

    // Ettention panel

    // Ettention params panel
    pnlEttentionsetupParams.setLayout(new BoxLayout(pnlEttentionsetupParams, BoxLayout.Y_AXIS));
    pnlEttentionsetupParams.setBorder(BorderFactory.createEtchedBorder());
    pnlEttentionsetupParams.add(ettentionSetupParamsHeader.getContainer());
    pnlEttentionsetupParams.add(pnlEttentionsetupParamsBody.getContainer());
    pnlEttentionsetupParams.add(Box.createRigidArea(FixedDim.x0_y5));
    pnlEttentionsetupParamsBody.setBoxLayout(BoxLayout.Y_AXIS);
    pnlEttentionsetupParamsBody.add(pnlETmethod1);
    pnlEttentionsetupParamsBody.add(pnlETmethod2);
    pnlEttentionsetupParamsBody.add(pnlBasicOptions);
    pnlEttentionsetupParamsBody.add(pnlUseLongObjectCompensation);
    pnlEttentionsetupParamsBody.add(pnlOversamplingOptions);
    pnlEttentionsetupParamsBody.add(pnlSubarea);
    pnlEttentionsetupParamsBody.add(pnlAdvancedParams);

   // pnlEttentionsetupParams.add(pnlEttentionsetupParamsBody.getContainer());
    pnlETmethod1.setLayout(new BoxLayout(pnlETmethod1, BoxLayout.X_AXIS));
    pnlETmethod1.add(rbETSart.getComponent());
    pnlETmethod1.add(rbETSirt.getComponent());
    pnlETmethod1.add(rbETOSSirt.getContainer());
    pnlETmethod2.setLayout(new BoxLayout(pnlETmethod2, BoxLayout.X_AXIS));
    pnlETmethod2.add(rbETPlugin.getComponent());
    pnlETmethod2.add(cmbETPlugin);

    // Ettention params body panel

    pnlBasicOptions.setLayout(new BoxLayout(pnlBasicOptions, BoxLayout.Y_AXIS));
    pnlBasicOptions.setAlignmentX(Box.CENTER_ALIGNMENT);
    pnlBasicOptions.add(ltfLambda.getComponent());
    pnlBasicOptions.add(ltfNumberOfIterations.getComponent());
    pnlBasicOptions.add(Box.createHorizontalGlue());

   // pnlEttentionsetupParams.add(pnlUseLongObjectCompensation);
    pnlUseLongObjectCompensation.setLayout(new BoxLayout(pnlUseLongObjectCompensation, BoxLayout.X_AXIS));
    pnlUseLongObjectCompensation.setAlignmentX(Box.CENTER_ALIGNMENT);
    pnlUseLongObjectCompensation.add(cbUseLongObjectCompensation);
    pnlUseLongObjectCompensation.add(Box.createHorizontalGlue());

    pnlOversamplingOptions.setLayout(new BoxLayout(pnlOversamplingOptions, BoxLayout.Y_AXIS));
    pnlOversamplingOptions.setAlignmentX(Box.CENTER_ALIGNMENT);
    pnlOversamplingOptions.add(ctfOversamplingForwardProjection.getComponent());
    pnlOversamplingOptions.add(ctfOversamplingBackProjection.getComponent());
    pnlOversamplingOptions.add(Box.createHorizontalGlue());

    pnlSubareaCB.setLayout(new BoxLayout(pnlSubareaCB, BoxLayout.X_AXIS));
    pnlSubareaCB.setAlignmentX(Box.CENTER_ALIGNMENT);
    pnlSubareaCB.add(cbSubarea);
    pnlSubareaCB.add(Box.createHorizontalGlue());

    // Subarea panel
    pnlSubarea.setLayout(new BoxLayout(pnlSubarea, BoxLayout.Y_AXIS));
    pnlSubarea.setBorder(BorderFactory.createEtchedBorder());
    pnlSubarea.setAlignmentX(Box.CENTER_ALIGNMENT);
    pnlSubarea.add(pnlSubareaCB);
    pnlSubarea.add(pnlSubareaSize);
    pnlSubarea.add(pnlCenter);

    // Offset and size panel

    pnlSubareaSize.setLayout(new BoxLayout(pnlSubareaSize, BoxLayout.X_AXIS));
    pnlSubareaSize.add(ltfSubareaSize.getContainer());
    pnlCenter.setLayout(new BoxLayout(pnlCenter, BoxLayout.X_AXIS));
    pnlCenter.add(ltfSubareaCenter.getContainer());
    pnlSubarea.add(Box.createHorizontalGlue());

    // create Panel with Plugin Parameters (from xml config)
    pnlAdvancedParams.setLayout(new BoxLayout(pnlAdvancedParams, BoxLayout.Y_AXIS));
    pnlPluginsContainer.setLayout(new BoxLayout(pnlPluginsContainer, BoxLayout.Y_AXIS));
    pnlPluginsContainer.add(Box.createRigidArea(FixedDim.x0_y5));
    createExtendedPanel(pnlAdvancedParams,pnlPluginsContainer);

   if(!pluginsPanels.isEmpty()){
    	pnlAvailablePlugins.setLayout(new BoxLayout(pnlAvailablePlugins, BoxLayout.Y_AXIS));
    	pnlAvailablePlugins.setBorder(BorderFactory.createEtchedBorder());
    	pnlAvailablePlugins.add(Box.createRigidArea(FixedDim.x0_y5));
    	pnlAvailablePlugins.add(pnlAvailablePluginsLabel);
    	pnlAvailablePlugins.add(pnlAvailablePluginsBody.getContainer());
    	pnlAvailablePluginsBody.setBoxLayout(BoxLayout.Y_AXIS);
    	pnlAvailablePluginsBody.add(pnlPluginsContainer);

    	pnlAvailablePluginsLabel.setLayout(new BoxLayout(pnlAvailablePluginsLabel, BoxLayout.X_AXIS));
    	pnlAvailablePluginsLabel.setAlignmentX(Box.CENTER_ALIGNMENT);
    	pnlAvailablePluginsLabel.add(Box.createRigidArea(FixedDim.x0_y5));
    	pnlAvailablePluginsLabel.add(new JLabel("Available plugins"));
    	//pnlAvailablePluginsLabel.add(Box.createHorizontalGlue());
    	fillComboBox();
    }
    else{
    	rbETPlugin.setEnabled(false);
    }

    // Buttons panel
    pnlButtons.setLayout(new BoxLayout(pnlButtons, BoxLayout.X_AXIS));
    pnlButtons.add(btnEttention.getComponent());
    pnlButtons.add(btn3dmodEttention.getComponent());
    pnlButtons.add(btnUseEttention.getComponent());

    // defaults
    rbETSart.setSelected(true);
    cbUseLongObjectCompensation.setSelected(true);
    cmbETPlugin.setEnabled(false);
    updateDisplay();
  }

  private void fillComboBox()
  {
	  for (String key : pluginsNames.keySet() )
      {
      	cmbETPlugin.addItem(key);
      }
  }

  private void addListeners() {
    btnEttention.addActionListener(listener);
    btn3dmodEttention.addActionListener(listener);
    btnUseEttention.addActionListener(listener);
    cbSubarea.addActionListener(listener);
    EttentionDocumentListener documentListener = new EttentionDocumentListener(this);
    ltfSubareaSize.addDocumentListener(documentListener);
    ltfSubareaCenter.addDocumentListener(documentListener);
    cmbETPlugin.addActionListener(listener);
    rbETPlugin.addActionListener(listener);
    rbETOSSirt.addActionListener(listener);
    rbETSirt.addActionListener(listener);
    rbETSart.addActionListener(listener);
  }

  void addResumeObserver(ResumeObserver resumeObserver) {
    resumeObservers.add(resumeObserver);
    //resumeChanged();
  }

  void addFieldObserver(FieldObserver fieldObserver) {
	    fieldObservers.add(fieldObserver);
	    fieldChangeAction();
	  }

	  final void fieldChangeAction() {
	    boolean diff = isDifferentFromCheckpoint();
	    Iterator<FieldObserver> i = fieldObservers.iterator();
	    while (i.hasNext()) {
	      i.next().msgFieldChanged(diff);
	    }
  }

  boolean isDifferentFromCheckpoint() {
	  return false;
  }

  public void msgFieldChanged(final boolean differentFromCheckpoint) {
    differentFromCheckpointFlag = differentFromCheckpoint;
    updateDisplay();
  }

  private void updateDisplay() {
	  boolean subarea = cbSubarea.isSelected();
	  ltfSubareaSize.setEnabled(subarea);
	  ltfSubareaCenter.setEnabled(subarea);
	  boolean pluginMethod = rbETPlugin.isSelected();
	  cmbETPlugin.setEnabled(pluginMethod);
  }

  void msgEttentionSucceeded() {
    loadResumeFrom();
  }

  void msgMethodChanged() {
    pnlRoot.setVisible(parent.isEttention());
  }

  void done() {
    btnEttention.removeActionListener(listener);
    btnUseEttention.removeActionListener(listener);
  }

  void getParameters(final ReconScreenState screenState) {
    btnEttention.setButtonState(screenState.getButtonState(btnEttention.getButtonStateKey()));
    btnUseEttention.setButtonState(screenState.getButtonState(btnUseEttention.getButtonStateKey()));
    ettentionSetupParamsHeader.getState(screenState.getTomoGenEttentionHeaderState());
  }

  void setParameters(final ReconScreenState screenState) {
	ettentionSetupParamsHeader.setState(screenState.getTomoGenEttentionHeaderState());
    btnEttention.setButtonState(screenState.getButtonState(btnEttention.getButtonStateKey()));
    btnUseEttention.setButtonState(screenState.getButtonState(btnUseEttention.getButtonStateKey()));
  }

  void getParameters(final MetaData metaData) {
  }

  void setParameters(final ConstMetaData metaData) {
  }


  public boolean getParameters(final EttentionsetupParam param, final boolean doValidation) {
    try {

        param.additionalParameter.clear();
        for (String key : extendedGUIParameter.keySet() )
        {
        	StringParameter parameter = new StringParameter( key );

        	Object component = extendedGUIParameter.get(key);
        	if ( component instanceof JTextField )
        	{
        		JTextField textField = (JTextField) component;
        		if(textField.getText().isEmpty()){
        			parameter.reset();
        		}
        		else{
        			parameter.set( textField.getText() );
        		}
        	}
        	if ( component instanceof JCheckBox )
        	{
        		JCheckBox checkBox = (JCheckBox) component;
        		if ( checkBox.isSelected() )
        			parameter.set( "true" );
        		else
        			parameter.set( "false" );
        	}
        	param.additionalParameter.addLast( parameter );
        }

        param.setLambda(ltfLambda.getText(doValidation));
        param.setNumberOfIterations(ltfNumberOfIterations.getText(doValidation));
        if (cbSubarea.isSelected()) {
            if (ltfSubareaSize.isEmpty()) {
              UIHarness.INSTANCE.openMessageDialog(manager, ltfSubareaSize.getLabel()
                  + " is empty.", "Entry Error", axisID);
              return false;
            }
            if (ltfSubareaCenter.isEmpty()) {
                UIHarness.INSTANCE.openMessageDialog(manager, ltfSubareaCenter.getLabel()
                    + " is empty.", "Entry Error", axisID);
                return false;
            }
            param.setSubareaSize(ltfSubareaSize.getText());
            param.setSubareaCenter(ltfSubareaCenter.getText());
          }
          else {
            param.resetSubareaSize();
            param.resetSubareaCenter();
        }
        if (ctfOversamplingForwardProjection.isSelected()) {
            if (ctfOversamplingForwardProjection.getText().isEmpty()) {
              UIHarness.INSTANCE.openMessageDialog(manager, ctfOversamplingForwardProjection.getLabel()
                  + " is empty.", "Entry Error", axisID);
              return false;
            }
            param.setOversamplingForwardProjection(ctfOversamplingForwardProjection.getText(doValidation));
          }
          else {
            param.resetOversamplingForwardProjection();
        }
        if (ctfOversamplingBackProjection.isSelected()) {
            if (ctfOversamplingBackProjection.getText().isEmpty()) {
              UIHarness.INSTANCE.openMessageDialog(manager, ctfOversamplingBackProjection.getLabel()
                  + " is empty.", "Entry Error", axisID);
              return false;
            }
            param.setOversamplingBackProjection(ctfOversamplingBackProjection.getText(doValidation));
          }
          else {
            param.resetOversamplingBackProjection();
        }

        param.setUseLongObjectCompensation(cbUseLongObjectCompensation.isSelected());
        param.setReconstructionType(getReconstructionType());
       /* if (rbETPlugin.isSelected()) {
        	if(rbETPlugin.getText().isEmpty()){
            UIHarness.INSTANCE.openMessageDialog(manager, rbETPlugin.getLabel()
                + " is empty.", "Entry Error", axisID);
            return false;
        	}
        }*/

        if(param.getReconstructionType().equals("blockIterative"))
        {
        	if (rbETOSSirt.getText().isEmpty()) {
                UIHarness.INSTANCE.openMessageDialog(manager, rbETOSSirt.getLabel()
                    + " is empty.", "Entry Error", axisID);
                return false;
             }
        	param.setNumberOfProjections(rbETOSSirt.getText(doValidation));
        }
        else {
            param.resetNumberOfProjections();
        }

        return true;
    }
    catch (FieldValidationFailedException e) {
      return false;
    }
  }

  public String getReconstructionType()
  {
	  if(rbETSart.isSelected())
	  {
		  return "sart";
	  }
	  else if(rbETSirt.isSelected())
	  {
		  return "sirt";
	  }
	  else if(rbETOSSirt.isSelected())
	  {
		  return "blockIterative";
	  }
	  else{// if(rbETPlugin)
		  String plugin = cmbETPlugin.getSelectedItem().toString();
		  String pluginID = pluginsNames.get(plugin);
	  	  return pluginID;
	  }
  }

  void setParameters(final EttentionsetupParam param) {

	  ltfLambda.setText(param.getLambda());
	  ltfNumberOfIterations.setText(param.getNumberOfIterations());
	  rbETOSSirt.setText(param.getNumberOfProjections());
      if (!param.isOversamplingForwardProjectionNull()) {
    	  ctfOversamplingForwardProjection.setText(param.getOversamplingForwardProjection());
      }
      ltfSubareaCenter.setText(param.getSubareaCenter());
      ltfSubareaCenter.setText("0.0, 0.0, 0.0");
	  updateDisplay();
  }

  void checkpoint(TomogramState state) {
	    updateDisplay();
  }

  /**
   * Load all of the .srecdd files in the current directory (any number of digits).  Save
   * the digits in the ResumeFromIteration pulldown list.
   */
  private void loadResumeFrom() {

  }

  /**
   * Ask the user to one or more files to open in 3dmod.  If only one file is available,
   * open in 3dmod without asking.
   * @param run3dmodMenuOptions
   */
  private void openFilesInImod(final Run3dmodMenuOptions run3dmodMenuOptions) {
    // Don't open the file chooser if there is only one file to choose
	EttentionOutputFileFilter ettentionOutputFileFilter = new EttentionOutputFileFilter(manager, axisID,
        true, true, true);
    File[] fileList = new File(manager.getPropertyUserDir())
        .listFiles((FilenameFilter) ettentionOutputFileFilter);
    if (fileList == null || fileList.length != 1) {
      JFileChooser chooser = new FileChooser(new File(manager.getPropertyUserDir()));
      chooser.setPreferredSize(UIParameters.INSTANCE.getFileChooserDimension());
      chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
      chooser.setMultiSelectionEnabled(true);
      chooser.setFileFilter(ettentionOutputFileFilter);
      int returnVal = chooser.showOpenDialog(pnlRoot);
      if (returnVal != JFileChooser.APPROVE_OPTION) {
        return;
      }
      fileList = chooser.getSelectedFiles();
      if (fileList == null || fileList.length == 0) {
        return;
      }
    }
    manager.openFilesInImod(axisID, ImodManager.ETTENTION_KEY, fileList, run3dmodMenuOptions);
  }

  /**
   * Ask the user to pick a file to use.  If only one file is available to pick, ask the
   * user if they want to use that file.
   */
  public void useEttention() {
	EttentionOutputFileFilter ettentionOutputFileFilter = new EttentionOutputFileFilter(manager, axisID,
        true, true, true);
    File[] fileList = new File(manager.getPropertyUserDir())
        .listFiles((FilenameFilter) ettentionOutputFileFilter);
    if (fileList != null && fileList.length == 1) {
      if (UIHarness.INSTANCE.openYesNoDialog(manager, "Use " + fileList[0].getName()
          + " as the tomogram?", axisID)) {
        manager.useEttention(btnUseEttention, fileList[0], btnEttention.getUnformattedLabel(), axisID,
            dialogType);
      }
      return;
    }
    JFileChooser chooser = new FileChooser(new File(manager.getPropertyUserDir()));
    chooser.setPreferredSize(UIParameters.INSTANCE.getFileChooserDimension());
    chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
    chooser.setFileFilter(ettentionOutputFileFilter);
    int returnVal = chooser.showOpenDialog(pnlRoot);
    if (returnVal != JFileChooser.APPROVE_OPTION) {
      return;
    }
    File file = chooser.getSelectedFile();
    if (file == null || !file.exists()) {
      return;
    }
    manager.useEttention(btnUseEttention, file, btnEttention.getUnformattedLabel(), axisID, dialogType);
  }

  void updateAdvanced(final boolean advanced) {
	  Iterator<JPanel> pnlIterator = advancedPanels.iterator();
	  while (pnlIterator.hasNext()) {
		  pnlIterator.next().setVisible(advanced);
	  }

  }

  public void expand(final GlobalExpandButton button) {
  }

  public void expand(final ExpandButton button) {
    if (ettentionSetupParamsHeader != null) {
      if (ettentionSetupParamsHeader.equalsOpenClose(button)) {
        pnlEttentionsetupParamsBody.setVisible(button.isExpanded());
      }
      else if (ettentionSetupParamsHeader.equalsAdvancedBasic(button)) {
        updateAdvanced(button.isExpanded());
      }
    }
    UIHarness.INSTANCE.pack(axisID, manager);
  }

  public void action(final Run3dmodButton button,
      final Run3dmodMenuOptions run3dmodMenuOptions) {
    action(button.getActionCommand(), button.getDeferred3dmodButton(),
        run3dmodMenuOptions);
  }


  /**
   * Executes the action associated with command.  Deferred3dmodButton is null
   * if it comes from the dialog's ActionListener.  Otherwise is comes from a
   * Run3dmodButton which called action(Run3dmodButton, Run3dmoMenuOptions).  In
   * that case it will be null unless it was set in the Run3dmodButton.
   * @param actionCommand
   * @param deferred3dmodButton
   * @param run3dmodMenuOptions
   */
  private void action(final String actionCommand,
      final Deferred3dmodButton deferred3dmodButton,
      final Run3dmodMenuOptions run3dmodMenuOptions) {
    if (actionCommand.equals(btnEttention.getActionCommand())) {
      manager.ettentionsetup(axisID, btnEttention, null, dialogType, parent.getProcessingMethod(),
          this);
    }
    else if (actionCommand.equals(btn3dmodEttention.getActionCommand())) {
      openFilesInImod(run3dmodMenuOptions);
    }
    else if (actionCommand.equals(btnUseEttention.getActionCommand())) {
      useEttention();
    }
    else if (actionCommand.equals(cbSubarea.getActionCommand()))
    {
    	updateDisplay();
    }
    else if (actionCommand.equals(rbETPlugin.getActionCommand()))
    {
    	updateDisplay();
    }
    else if (actionCommand.equals(rbETSart.getActionCommand()))
    {
    	updateDisplay();
    }
    else if (actionCommand.equals(rbETSirt.getActionCommand()))
    {
    	updateDisplay();
    }
    else if (actionCommand.equals(rbETOSSirt.getActionCommand()))
    {
    	updateDisplay();
    }
  }

  private void documentAction() {
    updateDisplay();
  }

  private void setToolTipText() {
    ReadOnlyAutodoc autodoc = null;
    try {
      autodoc = AutodocFactory.getInstance(manager, AutodocFactory.ETTENTIONSETUP, axisID);
    }
    catch (FileNotFoundException except) {
      except.printStackTrace();
    }
    catch (IOException except) {
      except.printStackTrace();
    }
    catch (LogFile.LockException e) {
      e.printStackTrace();
    }

    rbETSart.setToolTipText("Use SART method for reconstruction. Volume is updated after one projection.");
    rbETSirt.setToolTipText("Use SIRT method for reconstruction. Volume is updated after all projections.");
    rbETOSSirt.setToolTipText("Use Ordered Subset SIRT (OS-SIRT) method for reconstruction. Specify the number of projections for volume update.");
    rbETPlugin.setToolTipText("Use a reconstruction method from the plugin list. The list contains all available plugins that provide valid recosntruction method."
    		+ "The plugins that only provide some additional extensions (e.g. reading of a special input format etc.) are not listed here. They can be used"
    		+ "in combination with any reconstrucion method (predefined or from plugins) by filling in necessary parameters in the respective boxes below.");
    ctfOversamplingForwardProjection.setToolTipText("Use oversampling in forward projection step to improve the quality of reconstruction."
    		+ "The values should be squared numbers, i.e. 4, 9, 16 etc."
    		+ "Slows down reconstruction process. Default is 1 for no oversampling.");
    ctfOversamplingBackProjection.setToolTipText("Use oversampling in back projection step to improve the quality of reconstruction."
    		+ "The values should be squared numbers, i.e. 4, 9, 16 etc."
    		+ "Slows down reconstruction process. Default is 1 for no oversampling.");
    cbUseLongObjectCompensation.setToolTipText("Use if the scanned sample is larger than reconstructed area.");
    ltfNumberOfIterations.setToolTipText("Number of iterations to run.");
    cbSubarea.setToolTipText("To reconstruct only part of tomogram. All values have to be specified.");
    ltfSubareaSize.setToolTipText("Size of the subarea to be reconstructed (in binned voxels). Values can be between 1 and binned projection size for x and y, and greater than 1 for z.");
    ltfSubareaCenter.setToolTipText("Coordinates for the subarea center. The tomogram center is placed in 0,0,0. "
    		+"Enter positive value (in binned voxels) for an area on the right/above/behind from the tomogram center and negative values "
    		+"(in binned voxels) for an area on the left/below/in front of the tomogram. Enter 0, 0, 0 to keep the subarea in the center.");
    ltfLambda.setToolTipText("Relaxation parameter for back projection step - the value should be set"
    		+ "according to the noise level in input projections between 0.001 (high noise level)"
    		+ "and 2.0 (very low noise level).");
    btnEttention
        .setToolTipText("Run ettentionsetup, and then run the resulting .com files.");
    btnUseEttention
        .setToolTipText("Use an Ettention result as the tomogram (change the extension to .rec).");

  }

  private static final class EttentionActionListener implements ActionListener {
    private final EttentionPanel adaptee;

    private EttentionActionListener(final EttentionPanel adaptee) {
      this.adaptee = adaptee;
    }

    public void actionPerformed(final ActionEvent event) {
      adaptee.action(event.getActionCommand(), null, null);
    }
  }

  private static final class EttentionDocumentListener implements DocumentListener {
    private final EttentionPanel adaptee;

    private EttentionDocumentListener(final EttentionPanel adaptee) {
      this.adaptee = adaptee;
    }

    public void changedUpdate(final DocumentEvent event) {
      adaptee.documentAction();
    }

    public void insertUpdate(final DocumentEvent event) {
      adaptee.documentAction();
    }

    public void removeUpdate(final DocumentEvent event) {
      adaptee.documentAction();
    }
  }

}
