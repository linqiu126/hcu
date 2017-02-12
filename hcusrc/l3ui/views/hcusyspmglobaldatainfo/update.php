<?php

use yii\helpers\Html;

/* @var $this yii\web\View */
/* @var $model app\models\Hcusyspmglobaldatainfo */

$this->title = 'Update Hcusyspmglobaldatainfo: ' . ' ' . $model->sid;
$this->params['breadcrumbs'][] = ['label' => 'Hcusyspmglobaldatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = ['label' => $model->sid, 'url' => ['view', 'id' => $model->sid]];
$this->params['breadcrumbs'][] = 'Update';
?>
<div class="hcusyspmglobaldatainfo-update">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
