<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hculightstrbh1750datainfo */

$this->title = 'Create Hculightstrbh1750datainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hculightstrbh1750datainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hculightstrbh1750datainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
