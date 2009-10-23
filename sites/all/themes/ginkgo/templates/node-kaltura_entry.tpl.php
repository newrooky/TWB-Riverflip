<?php
// $Id: node-kaltura_entry.tpl.php,v 1.4 2008/09/15 08:11:49 johnalbin Exp $

/**
 * @file node-kaltura_entry.tpl.php
 *
 * Theme implementation to display a node.
 *
 * Available variables:
 * - $title: the (sanitized) title of the node.
 * - $content: Node body or teaser depending on $teaser flag.
 * - $picture: The authors picture of the node output from
 *   theme_user_picture().
 * - $date: Formatted creation date (use $created to reformat with
 *   format_date()).
 * - $links: Themed links like "Read more", "Add new comment", etc. output
 *   from theme_links().
 * - $name: Themed username of node author output from theme_user().
 * - $node_url: Direct url of the current node.
 * - $terms: the themed list of taxonomy term links output from theme_links().
 * - $submitted: themed submission information output from
 *   theme_node_submitted().
 *
 * Other variables:
 * - $node: Full node object. Contains data that may not be safe.
 * - $type: Node type, i.e. story, page, blog, etc.
 * - $comment_count: Number of comments attached to the node.
 * - $uid: User ID of the node author.
 * - $created: Time the node was published formatted in Unix timestamp.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   teaser listings.
 * - $id: Position of the node. Increments each time it's output.
 *
 * Node status variables:
 * - $teaser: Flag for the teaser state.
 * - $page: Flag for the full page state.
 * - $promote: Flag for front page promotion state.
 * - $sticky: Flags for sticky post setting.
 * - $status: Flag for published status.
 * - $comment: State of comment settings for the node.
 * - $readmore: Flags true if the teaser content of the node cannot hold the
 *   main body content.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 *
 * @see template_preprocess()
 * @see template_preprocess_node()
 */
?>

<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?>"><div class="node-inner">
  <?php if (!$page): ?>
    <h2 class="title">
      <a href="<?php print $node_url; ?>" title="<?php print $title ?>"><?php print $title; ?></a>
    </h2>
  <?php endif; ?>

  <?php if ($unpublished): ?>
    <div class="unpublished"><?php print t('Unpublished'); ?></div>
  <?php endif; ?>

  <div class="content">
    <?php /*print $content;*/ ?>
	<?php if ($teaser) : ?>
	<div id="general-videobox-<?php print $fields['kaltura_entryId']->content; ?>" class="general-videobox">
		<div class="general-videobox-inner">
			<div class="videobox-thumbnail">
				<?php print $fields['kaltura_thumbnail_url']->content; ?>
			</div>
			<div class="videobox-information">
				<div class="videobox-office">
					<?php print $fields['name']->content; ?>
				</div>
				<div class="videobox-title">
					<?php print $fields['title']->content; ?>
				</div>
				<div class="videobox-links">
					<a class="forward-page" title="Forward this page to a friend" href="/forward?path=node/<?php print $fields['nid']->content; ?>"><img class="forward-icon" height="16" width="16" title="" alt="Email this page" src="/<?php print drupal_get_path('theme', 'hometv_zen'); ?>/images/icons/email_link.png" /> <?php print t('Send to friend'); ?></a>
				</div>
			</div>
		</div>
	</div>
	<?php else : ?>
	<div class="videonode" id="videonode-<?php print $node->nid; ?>">
		<div class="videonode-inner">
			<div class="videonode-movie">
				<div class="videonode-movie-inner">
				<?php print kaltura_replace_tags($node->content['kaltura_entry']['#value'], FALSE, FALSE); ?>
				</div>
			</div>
			<div class="videonode-information">
				<div class="videonode-information-inner">
					<?php if ( $node->content['body']['#value'] ) : ?>
					<div class="videonode-description">
						<div class="videonode-label"><?php print t('Description'); ?></div>
						<div class="videonode-property"><?php print $node->content['body']['#value']; ?></div>
						<div class="clear-block"></div>
					</div>
					<?php endif; ?>
					<?php if ( $node->field_price[0]['safe'] ) : ?>
					<div class="videonode-price">
						<div class="videonode-label"><?php print t('Price'); ?></div>
						<div class="videonode-property"><?php print check_plain($node->field_price[0]['safe']); ?></div>
						<div class="clear-block"></div>
					</div>
					<?php endif; ?>
					<?php if ( $node->field_reference[0]['safe'] ) : ?>
					<div class="videonode-reference">
						<div class="videonode-label"><?php print t('Reference'); ?></div>
						<div class="videonode-property"><?php print check_plain($node->field_reference[0]['safe']); ?></div>
						<div class="clear-block"></div>
					</div>
					<?php endif; ?>
					<?php if ( $node->field_location[0]['safe'] ) : ?>
					<div class="videonode-location">
						<div class="videonode-label"><?php print t('Location'); ?></div>
						<div class="videonode-property">
						<?php 
						$locatie = hometv_dropdown_get_locatie($node->field_location[0]['safe']);
						print check_plain($locatie->pc_deelgemeente.', '.$locatie->deelgemeente); 
						?></div>
						<div class="clear-block"></div>
					</div>
					<?php endif; ?>
					<?php if ( $node->field_office[0]['url'] ) : ?>
					<div class="videonode-office">
						<div class="videonode-label"><?php print t('Immo Office'); ?></div>
						<div class="videonode-property">
						<a href="<?php print check_plain($node->field_office[0]['display_url'])?>" title="<?php print check_plain($node->field_office[0]['display_title'])?>" target="<?php print check_plain($node->field_office[0]['attributes']['target'])?>"><?php print check_plain($node->field_office[0]['display_title'])?></a>
						</div>
						<div class="clear-block"></div>
					</div>
					<?php endif; ?>
					<div class="videonode-links">
						<div class="videonode-label"><?php print ''; ?></div>
						<div class="videonode-property">
						<a class="forward-page" title="Forward this page to a friend" href="/forward?path=node/<?php print $node->nid; ?>"><img class="forward-icon" height="16" width="16" title="" alt="Email this page" src="/<?php print drupal_get_path('theme', 'hometv_zen'); ?>/images/icons/email_link.png" /> <?php print t('Send to friend'); ?></a>
						</div>
						<div class="clear-block"></div>
					</div>
					<div class="clear-block"></div>
				</div>
			</div>
			<div class="clear-block"></div>
		</div>
	</div>
  </div>
  <?php endif; ?>
</div></div> <!-- /node-inner, /node -->