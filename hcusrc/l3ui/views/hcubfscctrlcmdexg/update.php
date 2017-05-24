<?php

use yii\helpers\Html;

/* @var $this yii\web\View */
/* @var $model app\models\Hcubfscctrlcmdexg */

$this->title = 'Update Hcubfscctrlcmdexg: ' . ' ' . $model->sid;
$this->params['breadcrumbs'][] = ['label' => 'Hcubfscctrlcmdexgs', 'url' => ['index']];
$this->params['breadcrumbs'][] = ['label' => $model->sid, 'url' => ['view', 'id' => $model->sid]];
$this->params['breadcrumbs'][] = 'Update';
?>
<div class="hcubfscctrlcmdexg-update">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
