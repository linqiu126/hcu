<?php

use yii\helpers\Html;

/* @var $this yii\web\View */
/* @var $model app\models\Hcubfscstadatainfo */

$this->title = 'Update Hcubfscstadatainfo: ' . ' ' . $model->StaType;
$this->params['breadcrumbs'][] = ['label' => 'Hcubfscstadatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = ['label' => $model->StaType, 'url' => ['view', 'id' => $model->StaType]];
$this->params['breadcrumbs'][] = 'Update';
?>
<div class="hcubfscstadatainfo-update">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
