<?php

use yii\helpers\Html;

/* @var $this yii\web\View */
/* @var $model app\models\Hcutracemsgctr */

$this->title = 'Update Hcutracemsgctr: ' . ' ' . $model->msgid;
$this->params['breadcrumbs'][] = ['label' => 'Hcutracemsgctrs', 'url' => ['index']];
$this->params['breadcrumbs'][] = ['label' => $model->msgid, 'url' => ['view', 'id' => $model->msgid]];
$this->params['breadcrumbs'][] = 'Update';
?>
<div class="hcutracemsgctr-update">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
