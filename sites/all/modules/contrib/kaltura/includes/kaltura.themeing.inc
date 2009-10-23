<?php
// $Id: kaltura.themeing.inc,v 1.1.2.3 2009/01/14 12:46:17 kaltura Exp $

/*
 * @file
 * defines all themeing functions for kaltura core module
 *
 */
$GLOBALS['_kaltura_client_folder'] = drupal_get_path('module', 'kaltura') .'/kaltura_client/';

/*
 * helper function that is called in nodeAPI:alter
 *
 * this function gets the content to be displayed,
 * and returns the content with an embed tag instead of a "kaltura tag"
 *
 * some of the return content is a javascript with handler functions for the buttons on the player
 */
function kaltura_replace_tags($content, $is_comment = FALSE, $replace_to_thumb = FALSE) {
  global $user, $multibyte;
  $length = drupal_strlen($content);
  // add PHP_EOL before each kaltura widget to ensure correct grep_match results
  $content = str_replace('[kaltura-widget', PHP_EOL .'[kaltura-widget', $content);
  $found = FALSE;
  preg_match_all('/\[kaltura-widget(.*)\/\]/', $content, $matches);
  $kaltura_tags = array();
  foreach ($matches[0] as $key => $match) {
    if (strpos($match, '/][kal', $split_point) === FALSE) {
      $kaltura_tags[] = $matches[0][$key];
      continue;
    }
    $tags = explode('][', $match);
    $splitted = FALSE;
    if (count($tags)) {
      foreach ($tags as $tag_num => $tag) {
        $splitted = TRUE;
        if ($tag[0] != '[') {
          $tags[$tag_num] = '['. $tag;
        }
        if ($tag[drupal_strlen($tag) - 1] != ']') {
          $tags[$tag_num] .= ']';
        }
        $kaltura_tags[] = $tags[$tag_num];
      }
    }
  }

  foreach ($kaltura_tags as $kaltura_tag) {
    $found = TRUE;
    // parse the parameters from the tag
    $params = kaltura_get_params_from_tag($kaltura_tag);
  
    // get the embed options from the params
    $embed_options = kaltura_get_embed_options($params);
    
    $wid = $embed_options["wid"];
    $width = $embed_options["width"];
    $height = $embed_options["height"];
    $div_id = "kaltura_wrapper_". $wid;
    $thumbnail_div_id = "kaltura_thumbnail_". $wid;
    $player_id = "kaltura_player_". $wid;
    $partner_config = KalturaHelpers::getServiceConfiguration();
    $kaltura_server = (variable_get('kaltura_server_url', ''))? variable_get('kaltura_server_url', ''): KalturaSettings_SERVER_URL;

    $kaltura_poweredby = '<div style="width: '. $width .'px;" class="poweredByKaltura"><div><a target="_blank" href="http://drupal.kaltura.org">Video Module</a> by <a target="_blank" href="http://corp.kaltura.com">Kaltura</a></div></div>';
    if (variable_get('kaltura_display_credits', 1)) {
      $links = '<a href="http://corp.kaltura.com/download">open source video</a><a href="http://corp.kaltura.com/technology/">video platform</a>';
    }
    if ($replace_to_thumb === TRUE) {
      $html = '<img src="'. $kaltura_server .'/p/'. $partner_config->partnerId .'/sp/'. $partner_config->subPartnerId .'/thumbnail/entry_id/'. $embed_options["media_id"] .'" />';
      $content = str_replace($kaltura_tag, $html, $content);
      continue;
    }
    
    $div_id = "kaltura_wrapper_". $embed_options["media_id"];
    $player_id = ($embed_options['div_id'])? $embed_options['div_id']: "kaltura_player_". $embed_options["media_id"];
    $align = ($embed_options["align"])? 'text-align:'. $embed_options["align"] .'; ': '';
    if ($is_comment) {
      $thumb_div_id = 'kaltura_thumbnail_'. $embed_options["media_id"];
      $onclick_value = 'kaltura_activate_player(\''. $thumb_div_id .'\',\''. $div_id .'\');';
      $img_tag = '<img src="'. $kaltura_server .'/p/'. $partner_config->partnerId .'/sp/'. $partner_config->subPartnerId .'/thumbnail/entry_id/'. $embed_options["media_id"] .'/width/'. $width .'/height/'. $height .'/type/2/bgcolor/000000/crop_provider/wordpress_comment_placeholder" />';
      $comment_div = '<div id="'. $thumb_div_id .'" class="kaltura_hand" onclick="'. $onclick_value .'">';
      $comment_div .= $img_tag .'</div>';
    }
    else {
      $comment_div = '';
    }
    $html = $comment_div .'
      <div id="'. $div_id .'" class="kaltura_wrapper" style="'. (($comment_div != '')? 'display:none;':'') . $align . $embed_options['custom_style'] .'"'. $embed_options['js_events'] .'>'. $links .'</div>'. $kaltura_poweredby .'
      <script type="text/javascript">
              var kaltura_swf = new SWFObject("'. $embed_options["swfUrl"] .'", "'. $player_id .'", "'. $embed_options["width"] .'", "'. $embed_options["height"] .'", "9", "#000000");
              kaltura_swf.addParam("wmode", "opaque");
              kaltura_swf.addParam("flashVars", "'. $embed_options["flashVars"] . (($comment_div != '')? '&autoPlay=true': '') .'");
              kaltura_swf.addParam("allowScriptAccess", "always");
              kaltura_swf.addParam("allowFullScreen", "TRUE");
              kaltura_swf.addParam("allowNetworking", "all");
              kaltura_swf.write("'. $div_id .'");
      </script>
    ';
    
    // rebuild the html with our new code tag 
    $content = str_replace($kaltura_tag, $html, $content);
  }
  
  if ($found && $replace_to_thumb === FALSE) {
    $plugin_url = KalturaHelpers::getKalturaServerUrl();
    $js = '
      <script type="text/javascript">
        function onPlayerAddClick (kshowId,entryId,pd_extraData) {
          if (kshowId && kshowId != -1) 
            kalturaInitModalBox("'. url("kaltura/contribution_wizard/") .'" + kshowId);
          if (entryId && entryId != -1 && "true" == "'. (($embed_options['roughcut'])? 'true': '') .'")
            kalturaInitModalBox("'. url("kaltura/contribution_wizard/") . (($embed_options['roughcut'])? 'entry-': '') .'" + entryId );
        }
  
        function onPlayerEditClick (kshowId,entryId,pd_extraData) {
          if (kshowId && kshowId != -1 && "true" == "'. (($embed_options['kshow'])? 'true': '') .'") 
            kalturaInitModalBox("'. url("kaltura/simple_editor/") .'" + kshowId + "/kshow/user_id@'. $user->uid .'", { width: 890, height: 546 } );
          if (entryId && entryId != -1 && "true" == "'. (($embed_options['roughcut'])? 'true': '') .'")
            kalturaInitModalBox("'. url("kaltura/simple_editor/") .'" + entryId + "/entry/user_id@'. $user->uid .'", { width: 890, height: 546 } );
        }
  
        function handleGotoContribWizard (kshowId, pd_extraData) {
          kalturaInitModalBox("'. url("kaltura/contribution_wizard/") .'" + kshowId);
        }
  
        function handleGotoEditorWindow (kshowId, pd_extraData) {
          kalturaInitModalBox("'. url("kaltura/simple_editor/") .'" + kshowId, { width: 890, height: 546 } );
        }
      </script>';
  
    $content .= $js;
  }
  
  return $content;
}

/*
 * helper function that breaks the "kaltura tag" into parameters
 */
function kaltura_get_params_from_tag($tag) {
  $params = array();
  $attributes_array = explode(' ', $tag);
  for ($i = 1, $len = count($attributes_array); $i < $len; $i++) {
    $attr = $attributes_array[$i];
    if (!strpos($attr, "=")) {
      continue;
    }
    $attr = str_replace('"', "", $attr);
    $attr = str_replace("'", "", $attr);
    $keyvalue = explode('=', $attr);
    $key = $keyvalue[0];
    $value = $keyvalue[1];
    $params[$key] = $value;
  }
  return $params;
}

/*
 * callback function for kaltura/contribution_wizard
 *
 * Args is url are a PHP array encoded as JSON and url encoded
 * use kaltura_format_cw_vars() function to convert your array of variables to the correct string
 *
 * valid list of params: (required params marked with *, values in brackets)
 *   kshow_id* [-2, -1, 0, real_id]
 *   partner_data [free text]
 *   type [entry/null]
 *   context [field/field_mix/comment/null]
 *   field_id
 *   title
 */
function kaltura_contribution_wizard() {
  $args = func_get_args();
  $temp_args = unserialize(urldecode($args[0]));
  
  // argument handling
  foreach ($temp_args as $key => $val) {
    ${$key} = $val;
  }
  
  $kaltura_client = KalturaHelpers::getKalturaClient();
      
  if (!$kaltura_client) {
    drupal_set_message('Failed to start Kaltura session. Please check your settings.', 'warning');
    echo theme_kaltura_maintenance_page("<br /><a href=\"#\" onclick=\"window.top.kalturaCloseModalBox()\">Close</a>", TRUE);
    exit;
  }
  
  /* TODO: remove the creation of RC */
  if ($context == 'field_mix') {
    if (isset($args[1]) && $args[1] != '') {
      $title = $args[1];
    }
    // create roughcut at this point by API call,
    $rc = new KalturaEntry;
    $rc->name = $title;
    $session_user = KalturaHelpers::getSessionUser();
    $result = $kaltura_client->addRoughcutEntry($session_user, -2, $rc);
    // pass roughcut ID to CW.
    $kshow_id = $result['result']['entry']['id'];
    kaltura_create_node_from_roughcut($result['result'], 1);
  }
    
  $theme_params->height = 360;
  $theme_params->width = 680;
  $theme_params->swfUrl = KalturaHelpers::getContributionWizardUrl();
  $flash_vars = KalturaHelpers::getContributionWizardFlashVars($kaltura_client->getKs(), $kshow_id, $partner_data, $type, (($context == 'comment')? TRUE: FALSE));
  $theme_params->flashVarsStr = KalturaHelpers::flashVarsToString($flash_vars);
  
  if ($partner_data && strpos($partner_data, 'kshow_exist@yes')) {
    $add_new = TRUE;
    $theme_params->mix_id = kaltura_get_node_for_mix($kshow_id);
  }

  if ($context == 'field' || $context == 'field_mix' || $context == 'comment') {
    $no_collect_entries = ($context == 'field_mix')? TRUE: FALSE;
    echo theme('contribution_wizard_field', $theme_params, $field_id, $no_collect_entries, $kshow_id, $add_filter);
    exit();
  }
  if ($navigate_back === NULL) {
    $navigate_back = TRUE;
  }
  if ($kshow_id != "-2" && !$add_new) {
    echo theme('contribution_wizard', $theme_params);
  }
  else {
    echo theme('contribution_wizard_add', $theme_params, $navigate_back);
  }
  exit;
}

/*
 * theme the kaltura maintenance page
 *
 * this page is displayed whenever a try to create the $kaltura_client object fails
 */
function theme_kaltura_maintenance_page($text, $var) {
  return $text;
}

/*
 * theme for kaltura/contribution_wizard
 * in case we are adding entries to existing kshow
 */
function theme_contribution_wizard($theme_params) {
  $goto_url = kaltura_cw_destination();
  $javascript = '
    var goto_url = "'. $goto_url .'";
    function onContributionWizardAfterAddEntry() {
    }
    
    function onContributionWizardClose(modified) {
      if (modified == "0") { 
        //setTimeout("window.top.history.back();", 0);
        setTimeout("window.top.kalturaCloseModalBox();", 0);
      } else { 
        setTimeout("window.top.kalturaRefreshTop();", 0);
        // setTimeout("SendTopToEntriesPage();",0);
      }
      return;
    }
  ';
    
  $flash_embed = '
    <div id="divKalturaCw"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaCw", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaCw");
    </script>
  ';
  
  return theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
}

/*
 * theme for kaltura/contribution_wizard_add
 * in case we add entries as a standalone nodes or a new roughcut
 */
function theme_contribution_wizard_add($theme_params, $navigate_back = TRUE) {
  $goto_url = kaltura_cw_destination();
  $javascript = '
    var kshowId = -1;
    var goto_url = "'. $goto_url .'";
    var node_url = "'. (($theme_params->mix_id)? url("node/". $theme_params->mix_id): '') .'";
    function onContributionWizardAfterAddEntry(obj) {
      if (obj && obj.length > 0 && obj[0].kshowId)
      kshowId = obj[0].kshowId;
    }
    
    function onContributionWizardClose(modified) {
      if (modified == "0")';
      $javascript .= ($theme_params->mix_id)? 'setTimeout("SendTopToNodePage();", 0);': ($navigate_back)? 'setTimeout("window.top.history.back();", 0);': 'setTimeout("window.top.kalturaCloseModalBox();",0);';
      $javascript .= ' else ';
      $javascript .= ($theme_params->mix_id)? 'setTimeout("SendTopToNodePage();", 0);': 'setTimeout("SendTopToEntriesPage();",0);';
      $javascript .= '}';
    
  $flash_embed = '
    <div id="divKalturaCw"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaCw", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaCw");
    </script>
  ';

  return theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
}

/*
 * theme for kaltura/contribution_wizard_field
 * in case we add content as a CCK field
 */
function theme_contribution_wizard_field($theme_params, $field_id, $no_collect_entries, $kshow_id, $add_filter = 1) {
  $goto_url = kaltura_cw_destination();
  $javascript = '
    var entry_list = '. (($no_collect_entries === TRUE)? '"'. $kshow_id .'"': '-1') .';
    var goto_url = "'. $goto_url .'";
    var node_url = "'. (($theme_params->mix_id)? url("node/". $theme_params->mix_id): '') .'";
    function onContributionWizardAfterAddEntry(obj) {
      str = "";
      for(i=0;i<obj.length;i++) {
        str += obj[i]["entryId"] + ",";
      }
      '.
      (($no_collect_entries === TRUE)? '': 'if (entry_list == "-1") entry_list = str; else entry_list += str;') .'
    }
    
    function onContributionWizardClose(modified) {
      if (modified == "0")
        setTimeout("window.top.kalturaCloseModalBox();", 0);
      else
        window.location.href = "'. url("kaltura/insert_widget/") .'" + entry_list + "/'. $field_id .'";
    }
  ';

  $add_existing = '';
  $js  = '';
  if ($field_id != 'edit-comment') {
    $js = '$(document).ready(function() { window.top.document.getElementById("kaltura_modal_iframe").className = ""; ';
    $js .= PHP_EOL .'window.top.document.getElementById("kaltura_modal_iframe").scrolling = "no"; });';
    $js .= PHP_EOL .'window.top.document.getElementById("modalbox").style.height = "380px";';
    $js .= PHP_EOL .'window.top.document.getElementById("kaltura_modal_iframe").height = "380";';
    $add_existing = '<div id="media-upload-header">
          <ul id="sidemenu">
          <li id="tab-kaltura"><a class="current" href="'. url($_GET['q']) .'">Add Interactive Video</a></li>
          <li id="tab-kaltura_browse"><a href="'. url('kaltura/entries/allow-insert/'. $field_id, array( 'query' => array( 'filter' => $add_filter, 'cwurl' => url($_GET['q']) ) )) .'">Browse Interactive Videos</a></li>
          <li><a href="#" onclick="window.top.kalturaCloseModalBox();return false;">Close</a></li>
          </ul>
      </div>';
  }
  $flash_embed = '<div id="divKalturaCw"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaCw", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaCw");
    </script>
  ';

  return theme("kaltura_modal", array("javascript" => $javascript . $js, "flashEmbed" => $add_existing . $flash_embed));
}

/*
 * callback function for kaltura/simple_editor 
 */
function kaltura_simple_editor() {
  $args = func_get_args();
  $kshow_id = $args[0];
  $type = $args[1];
  $partner_data = ($args[2] != 'empty_partner_data')? $args[2]: '';
  $no_refresh = ($args[3] == 'no_refresh')? TRUE: FALSE;
  
  if (WORK_WITH_KSHOW) {
    $kaltura_client = KalturaHelpers::getKalturaClient(FALSE, "edit:". $kshow_id);
  }
  else {
    $kaltura_client = KalturaHelpers::getKalturaClient(FALSE, "edit:*");
  }

  
  if (!$kaltura_client) {
    drupal_set_message('Failed to start Kaltura session. Please check your settings.', 'warning');
    echo theme_kaltura_maintenance_page("<br /><a href=\"#\" onclick=\"window.top.kalturaCloseModalBox()\">Close</a>", TRUE);
    exit;
  }
  
  $theme_params->height = 546;
  $theme_params->width = 890;
  $theme_params->swfUrl = KalturaHelpers::getSimpleEditorUrl();
  $flash_vars = KalturaHelpers::getSimpleEditorFlashVars($kaltura_client->getKs(), $kshow_id, $type, $partner_data);
  $theme_params->flashVarsStr = KalturaHelpers::flashVarsToString($flash_vars);
  
  echo theme('simple_editor', $theme_params, $no_refresh);
  exit;
}

/*
 * theme for kaltura/simple_editor 
 */
function theme_simple_editor($theme_params, $no_refresh) {
  $javascript =' 
  function onSimpleEditorBackClick(modified) {
    if (modified == 0) { 
      setTimeout("window.top.kalturaCloseModalBox();", 0);
    } else {
    ';
  if ($no_refresh === TRUE) {
    $javascript .= 'setTimeout("window.top.kalturaCloseModalBox();", 0);';
  }
  else {
    $javascript .= 'setTimeout("window.top.kalturaRefreshTop();", 0);';
  }
  $javascript .= '
    }
    return;
  }
  
  function onSimpleEditorSaveClick(modified) {
  
  }
  ';

  $flash_embed = '
    <div id="divKalturaSe"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaSe", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaSe");
    </script>
  ';

  return theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
}

/*
 * main theme for modal windows 
 */
function theme_kaltura_modal($theme_params) {
  return '
    <html>
    <head>
    <link rel="stylesheet" type="text/css" href="'. base_path() . drupal_get_path('module', 'kaltura') .'/style/kaltura.css"/>
    <style type="text/css">
            html, body { margin:0; padding:0; }
    </style>
    <script type="text/javascript" src="'. base_path() . drupal_get_path('module', 'kaltura') .'/js/kaltura.js"></script>
    <script type="text/javascript" src="'. base_path() .'misc/jquery.js"></script>
    <script type="text/javascript" src="'. base_path() . drupal_get_path('module', 'kaltura') .'/js/swfobject.js"></script>
    <script type="text/javascript">
      '. $theme_params["javascript"] .'    
    </script>
    </head>
    <body>
      '. $theme_params["flashEmbed"] .'
    </body>
    </html>';
}

/*
 * callback function for kaltura/insert_widget page
 * 
 * currently not in use
 * 
 * used in old version of the module.
 * might be useful for the "kaltura_inline" implementation
 */
function kaltura_insert_widget() {
  global $_kaltura_thumbnail_base_url;
  $args = func_get_args();
  $entry_list = $args[0];
  $field_name = str_replace('_', '-', $args[1]);
  $fix_subject = '';
  if ($field_name == 'edit-comment') {
    $fix_subject = '      if (window.top.document.getElementById("edit-subject").value == "") ';
    $fix_subject .= '       window.top.document.getElementById("edit-subject").value = "video comment";';
    $entries = explode(',', rtrim($entry_list, ','));
    $entry_list = '';
    foreach ($entries as $entry) {
      $entry_list .= '[kaltura-widget media_type=\"video\" comment=\"'. $entry .'\" size=\"large\" /]';
    }
  }
  
  $javascript =' 
    var bodyTextArea = window.top.document.getElementById("'. $field_name .'");
    if (bodyTextArea)
      bodyTextArea.value += "'. $entry_list .'";
      '. $fix_subject;
      $javascript .= ($field_name == 'edit-comment')? 'window.top.document.getElementById("edit-preview").click();': '';
      $javascript .= ($field_name != 'edit-comment')? 'update_field_thumbs(window.top.document.getElementById(\''. $field_name .'\'), \''. $_kaltura_thumbnail_base_url .'\');': '';
      $javascript .= '
      window.top.kalturaCloseModalBox();
  ';

  $flash_embed = '';
  
  echo theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
  exit;
}

/*
 * helper function to get embed options according to the "kaltura tag" parameters
 *
 * this function also calls the kaltura_use_widget (file kaltura.module) function which exposes a hook
 *
 * returns an array of embed options later injected into the embed tag
 */
function kaltura_get_embed_options($params) {
  switch ($params["align"]) {
    case 'r': $align = "right";
      break;
    case 'm': $align = "center";
      break;
    case 'l': $align = "left";
      break;
    default: $align = "";
      break;
  }

  if ($params['id']) {
    $div_id = $params['id'];
  }
  if ($params['custom_style']) {
    $custom_style = $params['custom_style'];
  }
  
  if ($_SERVER["SERVER_PORT"] == 443) {
    $protocol = "https://";
  }
  else {
    $protocol = "http://"; 
  }


  if ($params['uiconf_id']) {
    $uicid = $params['uiconf_id'];
    $theme_uicid = TRUE;
  }
  $wid = '_'. variable_get('kaltura_partner_id', '');

  $widescreen = '';
  if ($params["entry"]) {
    $uicid = kaltura_use_uiconf($uicid, 'entry', $theme_uicid, $params['media_type']);
    $swf_url = KalturaHelpers::getSwfUrlForWidget($wid .'_'. $uicid .'/entry_id/'. $params["entry"] .'/uiconf_id/'. $uicid);
    $media_id = $params["entry"];
    if ($params['media_type'] == 'video') {
      if (variable_get('kaltura_video_entry_player_ratio', 0) == 1) {
        $widescreen = '&widescreen=1';
      }
    }
    $player_size = kaltura_calculate_player_size('entry', $params['size'], $params['width'], $params['height']);
  }
  elseif ($params["kid"]) {
    $uicid = kaltura_use_uiconf($uicid, 'mix', $theme_uicid);
    $kshow = TRUE;
    $swf_url = KalturaHelpers::getSwfUrlForWidget($wid .'_'. $uicid .'/kid/'. $params["kid"] .'/uiconf_id/'. $uicid);
    $media_id = $params["kid"];
    $player_size = kaltura_calculate_player_size('mix', $params['size'], $params['width'], $params['height']);
  }
  elseif ($params["mix"]) {
    $uicid = kaltura_use_uiconf($uicid, 'mix', $theme_uicid);
    $roughcut = TRUE;
    $swf_url = KalturaHelpers::getSwfUrlForWidget($wid .'_'. $uicid .'/entry_id/'. $params["mix"] .'/uiconf_id/'. $uicid);
    $media_id = $params["mix"];
    $player_size = kaltura_calculate_player_size('mix', $params['size'], $params['width'], $params['height']);
  }
  elseif ($params["comment"]) {
    $uicid = kaltura_use_uiconf($uicid, 'entry', $theme_uicid, 'comment');
    $swf_url = KalturaHelpers::getSwfUrlForWidget($wid .'_'. $uicid .'/entry_id/'. $params["comment"] .'/uiconf_id/'. $uicid);
    $media_id = $params["comment"];
    $player_size = kaltura_calculate_player_size('comment', $params['size'], $params['width'], $params['height']);
  }

  $finish_f = '';
  if ($params['finishF']) {
    $finish_f = '&finishF='. $params['finishF'];
  }
  $flash_vars_str = "layoutId=". $player_size['layout_id'] ."&pd_original_url=". urlencode($protocol . $_SERVER["HTTP_HOST"] . request_uri()) . $widescreen . $finish_f;

  return array(
    "flashVars" => $flash_vars_str,
    "height" => $player_size["height"],
    "width" => $player_size["width"],
    "custom_style" => $custom_style,
    "align" => $align,
    "media_id" => $media_id,
    "js_events" => $events,
    "wid" => $wid,
    "uiconf" => $uicid,
    "roughcut" => $roughcut,
    "kshow" => $kshow,
    "swfUrl" => $swf_url,
    "div_id" => $div_id
  );
}

/*
 * helper function that returns the player width and height according to letter
 * found in the "kaltura tag"
 */
function kaltura_calculate_player_size($type, $size, $width = 0, $height = 0) {
  if ($width > 0 && $height > 0) {
    return array('width' => $width, 'height' => $height, 'layout_id' => 'fullLarge');
  }
  switch ($type) {
    case 'entry':
      $variable_width = variable_get('kaltura_entry_width', '410');
      $variable_height = variable_get('kaltura_entry_height', '364');
      break;
    case 'mix':
      $variable_width = variable_get('kaltura_mix_width', '410');
      $variable_height = variable_get('kaltura_mix_height', '364');
      break;
    case 'comment':
      $variable_width = variable_get('kaltura_comment_width', '250');
      $variable_height = variable_get('kaltura_comment_height', '244');
      break;
  }
  return array('width' => $variable_width, 'height' => $variable_height, 'layout_id' => 'fullLarge');
}
