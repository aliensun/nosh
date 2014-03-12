<?php
/*
 * @file
 * This template is used for make style for foursquare result.
 */
?>
<div id="foursquare_container">
    <?php
    foreach ($foursqure_tips_var as $foursqure_tips_key1) {
        $foursqure_tip_id = $foursqure_tips_array['tipid'][$foursqure_tips_key1];
        $foursqure_tip_photo = $foursqure_tips_array['photo'][$foursqure_tip_id];
        $foursqure_tip_text = $foursqure_tips_array['text'][$foursqure_tip_id];
        $foursqure_tip_venue = $foursqure_tips_array['venue'][$foursqure_tip_id];
        $foursqure_tip_timestamp = $foursqure_tips_array['timestamp'][$foursqure_tip_id];
        $foursqure_tips_date = format_interval((time() - $foursqure_tip_timestamp) , 2) . t(' ago'); //time_ago($foursqure_tip_timestamp);
    ?>
        <div id='foursqure_container_tips'>
            <div id='foursqure_container_tipsimage'>
                <img src='<?php print $foursqure_tip_photo; ?>'></div>
            <div id='foursqure_container_tipscontent'><p class='foursqure_container_vname'><?php print $foursqure_tip_venue; ?></p><?php print $foursqure_tip_text; ?><br />
                <span class='foursqure_container_timeago'><?php print $foursqure_tips_date; ?></span></div>
        </div><div class='clear'></div><hr>
    <?php } ?>
</div>

