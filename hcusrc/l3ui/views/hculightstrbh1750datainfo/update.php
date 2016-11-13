<?php

use yii\helpers\Html;

/* @var $this yii\web\View */
/* @var $model app\models\Hculightstrbh1750datainfo */

$this->title = 'Update Hculightstrbh1750datainfo: ' . ' ' . $model->sid;
$this->params['breadcrumbs'][] = ['label' => 'Hculightstrbh1750datainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = ['label' => $model->sid, 'url' => ['view', 'id' => $model->sid]];
$this->params['breadcrumbs'][] = 'Update';
?>
<div class="hculightstrbh1750datainfo-update">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
